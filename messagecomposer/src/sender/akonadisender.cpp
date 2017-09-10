/*
 * This file is part of KMail.
 * Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "akonadisender.h"

#include "messagecomposer_debug.h"

#include "helper/messagehelper.h"
#include "settings/messagecomposersettings.h"
#include "MessageComposer/Util"
#include "utils/util_p.h"

#include <kmime/kmime_message.h>
#include <libkdepim/progressmanager.h>

#include <mailtransportakonadi/dispatcherinterface.h>
#include <mailtransportakonadi/messagequeuejob.h>
#include <mailtransport/transport.h>
#include <mailtransport/transportmanager.h>
#include <MessageCore/StringUtil>

using namespace KMime::Types;
using namespace KPIM;
using namespace MailTransport;
using namespace MessageComposer;

static QStringList addrSpecListToStringList(const AddrSpecList &l, bool allowEmpty = false)
{
    QStringList result;
    for (AddrSpecList::const_iterator it = l.constBegin(), end = l.constEnd(); it != end; ++it) {
        const QString s = (*it).asString();
        if (allowEmpty || !s.isEmpty()) {
            result.push_back(s);
        }
    }
    return result;
}

static void extractSenderToCCAndBcc(const KMime::Message::Ptr &aMsg, QString &sender, QStringList &to, QStringList &cc, QStringList &bcc)
{
    sender = aMsg->sender()->asUnicodeString();
    if (aMsg->headerByType("X-KMail-Recipients")) {
        // extended BCC handling to prevent TOs and CCs from seeing
        // BBC information by looking at source of an OpenPGP encrypted mail
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "X-KMail-Recipients"));
        aMsg->removeHeader("X-KMail-Recipients");
    } else if (aMsg->headerByType("Resent-To")) {
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-To"));
        cc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-Cc"));
        bcc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Resent-Bcc"));
    } else {
        to = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "To"));
        cc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Cc"));
        bcc = addrSpecListToStringList(MessageHelper::extractAddrSpecs(aMsg, "Bcc"));
    }
}

class MessageComposer::AkonadiSenderPrivate
{
public:
    AkonadiSenderPrivate()
        : mCustomTransportId(-1)
    {
    }

    QSet<KJob *> mPendingJobs;
    int mCustomTransportId;
};

AkonadiSender::AkonadiSender(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::AkonadiSenderPrivate)
{
}

AkonadiSender::~AkonadiSender()
{
    delete d;
}

bool AkonadiSender::doSend(const KMime::Message::Ptr &aMsg, short sendNow)
{
    if (sendNow == -1) {
        sendNow = MessageComposer::MessageComposerSettings::self()->sendImmediate(); // -1 == use default setting
    }
    if (sendNow) {
        sendOrQueueMessage(aMsg, MessageComposer::MessageSender::SendImmediate);
    } else {
        sendOrQueueMessage(aMsg, MessageComposer::MessageSender::SendLater);
    }
    return true;
}

bool AkonadiSender::doSendQueued(int customTransportId)
{
    qCDebug(MESSAGECOMPOSER_LOG) << "Sending queued message with custom transport:" << customTransportId;
    if (!MessageComposer::Util::sendMailDispatcherIsOnline()) {
        return false;
    }

    d->mCustomTransportId = customTransportId;

    DispatcherInterface *dispatcher = new DispatcherInterface();
    if (d->mCustomTransportId == -1) {
        dispatcher->dispatchManually();
    } else {
        dispatcher->dispatchManualTransport(d->mCustomTransportId);
    }
    delete dispatcher;
    return true;
}

void AkonadiSender::sendOrQueueMessage(const KMime::Message::Ptr &message, MessageComposer::MessageSender::SendMethod method)
{
    Q_ASSERT(message);
    qCDebug(MESSAGECOMPOSER_LOG) << "KMime::Message: \n[\n" << message->encodedContent().left(1000) << "\n]\n";

    MessageQueueJob *qjob = new MessageQueueJob(this);
    if (message->hasHeader("X-KMail-FccDisabled")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(MailTransport::SentBehaviourAttribute::Delete);
    } else if (auto hrd = message->headerByType("X-KMail-Fcc")) {
        qjob->sentBehaviourAttribute().setSentBehaviour(
            SentBehaviourAttribute::MoveToCollection);
        const int sentCollectionId = hrd->asUnicodeString().toInt();
        qjob->sentBehaviourAttribute().setMoveToCollection(
            Akonadi::Collection(sentCollectionId));
    } else {
        qjob->sentBehaviourAttribute().setSentBehaviour(
            MailTransport::SentBehaviourAttribute::MoveToDefaultSentCollection);
    }
    qjob->setMessage(message);

    // Get transport.
    int transportId = -1;
    if (d->mCustomTransportId != -1) {
        transportId = d->mCustomTransportId;
    } else {
        if (auto hrd = message->headerByType("X-KMail-Transport")) {
            transportId = hrd->asUnicodeString().toInt();
        }
    }
    const Transport *transport = TransportManager::self()->transportById(transportId);
    if (!transport) {
        qCDebug(MESSAGECOMPOSER_LOG) << " No transport defined. Need to create it";
        return;
    }
    if ((method == MessageComposer::MessageSender::SendImmediate) && !MessageComposer::Util::sendMailDispatcherIsOnline()) {
        return;
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "Using transport (" << transport->name() << "," << transport->id() << ")";
    qjob->transportAttribute().setTransportId(transport->id());

    // if we want to manually queue it for sending later, then do it
    if (method == MessageComposer::MessageSender::SendLater) {
        qjob->dispatchModeAttribute().setDispatchMode(MailTransport::DispatchModeAttribute::Manual);
    }

    // Get addresses.
    QStringList to, cc, bcc;
    QString from;
    extractSenderToCCAndBcc(message, from, to, cc, bcc);
    qjob->addressAttribute().setFrom(from);
    qjob->addressAttribute().setTo(to);
    qjob->addressAttribute().setCc(cc);
    qjob->addressAttribute().setBcc(bcc);

    MessageComposer::Util::addSendReplyForwardAction(message, qjob);

    MessageCore::StringUtil::removePrivateHeaderFields(message, false);
    message->assemble();

    // Queue the message.
    connect(qjob, &MessageQueueJob::result, this, &AkonadiSender::queueJobResult);
    d->mPendingJobs.insert(qjob);
    qjob->start();
    qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob started.";

    // TODO potential problem:
    // The MDA finishes sending a message before I queue the next one, and
    // thinking it is finished, the progress item deletes itself.
    // Turn the MDA offline until everything is queued?
}

void AkonadiSender::queueJobResult(KJob *job)
{
    Q_ASSERT(d->mPendingJobs.contains(job));
    d->mPendingJobs.remove(job);

    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob failed with error" << job->errorString();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "QueueJob success.";
    }
}
