/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "settings/messagecomposersettings.h"
#include "job/skeletonmessagejob.h"
#include "messagecomposer-version.h"

#include "part/infopart.h"
#include "part/globalpart.h"
#include "job/jobbase_p.h"

#include <QHostInfo>
#include <QUrl>

#include <KCharsets>
#include "messagecomposer_debug.h"

#include <kmime/kmime_message.h>
#include <KEmailAddress>

using namespace MessageComposer;

class MessageComposer::SkeletonMessageJobPrivate : public JobBasePrivate
{
public:
    SkeletonMessageJobPrivate(SkeletonMessageJob *qq)
        : JobBasePrivate(qq)
    {
    }

    void doStart(); // slot

    InfoPart *infoPart = nullptr;
    GlobalPart *globalPart = nullptr;
    KMime::Message *message = nullptr;

    Q_DECLARE_PUBLIC(SkeletonMessageJob)
};

void SkeletonMessageJobPrivate::doStart()
{
    Q_Q(SkeletonMessageJob);

    Q_ASSERT(infoPart);
    Q_ASSERT(message == nullptr);
    message = new KMime::Message;

    // From:
    {
        KMime::Headers::From *from = new KMime::Headers::From;
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(infoPart->from()));
        from->fromUnicodeString(QString::fromLatin1(address.as7BitString("utf-8")), "utf-8");
        message->setHeader(from);
    }

    // To:
    {
        KMime::Headers::To *to = new KMime::Headers::To;
        QByteArray sTo;
        const QStringList lstTo = infoPart->to();
        for (const QString &a : lstTo) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sTo.isEmpty()) {
                sTo.append(",");
            }
            sTo.append(address.as7BitString("utf-8"));
        }
        to->fromUnicodeString(QString::fromLatin1(sTo), "utf-8");
        message->setHeader(to);
    }

    // Reply To:
    if (!infoPart->replyTo().isEmpty()) {
        KMime::Headers::ReplyTo *replyTo = new KMime::Headers::ReplyTo;
        const QStringList lstReplyTo = infoPart->replyTo();
        QByteArray sReplyTo;
        for (const QString &a : lstReplyTo) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sReplyTo.isEmpty()) {
                sReplyTo.append(",");
            }
            sReplyTo.append(address.as7BitString("utf-8"));
        }
        replyTo->fromUnicodeString(QString::fromLatin1(sReplyTo), "utf-8");
        message->setHeader(replyTo);
    }

    // Cc:
    {
        KMime::Headers::Cc *cc = new KMime::Headers::Cc;
        QByteArray sCc;
        const QStringList lstCc = infoPart->cc();
        for (const QString &a : lstCc) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sCc.isEmpty()) {
                sCc.append(",");
            }
            sCc.append(address.as7BitString("utf-8"));
        }
        cc->fromUnicodeString(QString::fromLatin1(sCc), "utf-8");
        message->setHeader(cc);
    }

    // Bcc:
    {
        KMime::Headers::Bcc *bcc = new KMime::Headers::Bcc;
        QByteArray sBcc;
        const QStringList lstBcc = infoPart->bcc();
        for (const QString &a : lstBcc) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sBcc.isEmpty()) {
                sBcc.append(",");
            }
            sBcc.append(address.as7BitString("utf-8"));
        }
        bcc->fromUnicodeString(QString::fromLatin1(sBcc), "utf-8");
        message->setHeader(bcc);
    }

    // Subject:
    {
        KMime::Headers::Subject *subject = new KMime::Headers::Subject;
        subject->fromUnicodeString(infoPart->subject(), "utf-8");
        // TODO should we be more specific about the charset?
        message->setHeader(subject);
    }

    // Date:
    {
        KMime::Headers::Date *date = new KMime::Headers::Date;
        date->setDateTime(QDateTime::currentDateTime());
        message->setHeader(date);
    }

    // Fcc:
    if (!infoPart->fcc().isEmpty()) {
        KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(infoPart->fcc(), "utf-8");
        message->setHeader(header);
    }

    //Transport:
    if (infoPart->transportId() > -1) {
        KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(QString::number(infoPart->transportId()), "utf-8");
        message->setHeader(header);
    }

    // Message-ID
    {
        KMime::Headers::MessageID *messageId = new KMime::Headers::MessageID();
        QByteArray fqdn;
        if (MessageComposer::MessageComposerSettings::self()->useCustomMessageIdSuffix()) {
            fqdn = QUrl::toAce(MessageComposer::MessageComposerSettings::self()->customMsgIDSuffix());
        }
        if (fqdn.isEmpty()) {
            fqdn = QUrl::toAce(QHostInfo::localHostName());
        }
        if (fqdn.isEmpty()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Unable to generate a Message-ID, falling back to 'localhost.localdomain'.";
            fqdn = "local.domain";
        }
        messageId->generate(fqdn);
        message->setHeader(messageId);
    }
    // Extras

    foreach (KMime::Headers::Base *extra, infoPart->extraHeaders()) {
        message->setHeader(extra);
    }

    // Request Delivery Confirmation
    {
        if (globalPart->requestDeleveryConfirmation()) {
            //TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            KMime::Headers::Generic *requestDeleveryConfirmation = new KMime::Headers::Generic("Return-Receipt-To");
            requestDeleveryConfirmation->fromUnicodeString(addr, "utf-8");
            message->setHeader(requestDeleveryConfirmation);
        }
    }

    // MDN
    {
        if (globalPart->MDNRequested()) {
            //TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            KMime::Headers::Generic *mdn = new KMime::Headers::Generic("Disposition-Notification-To");
            mdn->fromUnicodeString(addr, "utf-8");
            message->setHeader(mdn);
        }
    }

    // Urgent header
    if (infoPart->urgent()) {
        KMime::Headers::Generic *urg1 = new KMime::Headers::Generic("X-PRIORITY");
        urg1->fromUnicodeString(QStringLiteral("2 (High)"), "utf-8");
        KMime::Headers::Generic *urg2 = new KMime::Headers::Generic("Priority");
        urg2->fromUnicodeString(QStringLiteral("urgent"), "utf-8");
        message->setHeader(urg1);
        message->setHeader(urg2);
    }

    // In-Reply-To
    if (!infoPart->inReplyTo().isEmpty()) {
        KMime::Headers::InReplyTo *header = new KMime::Headers::InReplyTo;
        header->fromUnicodeString(infoPart->inReplyTo(), "utf-8");
        message->setHeader(header);
    }

    // References
    if (!infoPart->references().isEmpty()) {
        KMime::Headers::References *header = new KMime::Headers::References;
        header->fromUnicodeString(infoPart->references(), "utf-8");
        message->setHeader(header);
    }

    q->emitResult(); // Success.
}

SkeletonMessageJob::SkeletonMessageJob(InfoPart *infoPart, GlobalPart *globalPart, QObject *parent)
    : JobBase(*new SkeletonMessageJobPrivate(this), parent)
{
    Q_D(SkeletonMessageJob);
    d->infoPart = infoPart;
    d->globalPart = globalPart;
}

SkeletonMessageJob::~SkeletonMessageJob()
{
}

InfoPart *SkeletonMessageJob::infoPart() const
{
    Q_D(const SkeletonMessageJob);
    return d->infoPart;
}

void SkeletonMessageJob::setInfoPart(InfoPart *part)
{
    Q_D(SkeletonMessageJob);
    d->infoPart = part;
}

GlobalPart *SkeletonMessageJob::globalPart() const
{
    Q_D(const SkeletonMessageJob);
    return d->globalPart;
}

void SkeletonMessageJob::setGlobalPart(GlobalPart *part)
{
    Q_D(SkeletonMessageJob);
    d->globalPart = part;
}

KMime::Message *SkeletonMessageJob::message() const
{
    Q_D(const SkeletonMessageJob);
    return d->message;
}

void SkeletonMessageJob::start()
{
    Q_D(SkeletonMessageJob);
    d->doStart();
}

#include "moc_skeletonmessagejob.cpp"
