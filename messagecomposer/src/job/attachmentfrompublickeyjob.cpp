/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by:
  Various authors.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "attachmentfrompublickeyjob.h"

#include <KDialogJobUiDelegate>
#include <KLocalizedString>

#include <QGpgME/Protocol>
#include <QGpgME/ExportJob>

#include <Libkleo/ProgressDialog>

using namespace MessageComposer;
using MessageCore::AttachmentPart;

class Q_DECL_HIDDEN MessageComposer::AttachmentFromPublicKeyJob::Private
{
public:
    Private(AttachmentFromPublicKeyJob *qq);

    void exportResult(const GpgME::Error &error, const QByteArray &keyData);   // slot
    void emitGpgError(const GpgME::Error &error);

    AttachmentFromPublicKeyJob *const q;
    QString fingerprint;
    QByteArray data;
};

AttachmentFromPublicKeyJob::Private::Private(AttachmentFromPublicKeyJob *qq)
    : q(qq)
{
}

void AttachmentFromPublicKeyJob::Private::exportResult(const GpgME::Error &error, const QByteArray &keyData)
{
    if (error) {
        emitGpgError(error);
        return;
    }

    // Create the AttachmentPart.
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(i18n("OpenPGP key 0x%1", fingerprint.right(8)));
    part->setFileName(QString::fromLatin1(QByteArray(QByteArray("0x") + fingerprint.toLatin1() + QByteArray(".asc"))));
    part->setMimeType("application/pgp-keys");
    part->setData(keyData);

    q->setAttachmentPart(part);
    q->emitResult(); // Success.
}

void AttachmentFromPublicKeyJob::Private::emitGpgError(const GpgME::Error &error)
{
    Q_ASSERT(error);
    const QString msg = i18n("<p>An error occurred while trying to export "
                             "the key from the backend:</p>"
                             "<p><b>%1</b></p>",
                             QString::fromLocal8Bit(error.asString()));
    q->setError(KJob::UserDefinedError);
    q->setErrorText(msg);
    q->emitResult();
}

AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJob(const QString &fingerprint, QObject *parent)
    : AttachmentLoadJob(parent)
    , d(new Private(this))
{
    d->fingerprint = fingerprint;
}

AttachmentFromPublicKeyJob::~AttachmentFromPublicKeyJob()
{
    delete d;
}

QString AttachmentFromPublicKeyJob::fingerprint() const
{
    return d->fingerprint;
}

void AttachmentFromPublicKeyJob::setFingerprint(const QString &fingerprint)
{
    d->fingerprint = fingerprint;
}

void AttachmentFromPublicKeyJob::doStart()
{
    QGpgME::ExportJob *job = QGpgME::openpgp()->publicKeyExportJob(true);
    Q_ASSERT(job);
    connect(job, &QGpgME::ExportJob::result, this, [this](const GpgME::Error &error, const QByteArray &ba) {
        d->exportResult(error, ba);
    });

    const GpgME::Error error = job->start(QStringList(d->fingerprint));
    if (error) {
        d->emitGpgError(error);
        // TODO check autodeletion policy of Kleo::Jobs...
        return;
    } else if (uiDelegate()) {
        Q_ASSERT(dynamic_cast<KDialogJobUiDelegate *>(uiDelegate()));
        KDialogJobUiDelegate *delegate = static_cast<KDialogJobUiDelegate *>(uiDelegate());
        (void)new Kleo::ProgressDialog(job, i18n("Exporting key..."),
                                       delegate->window());
    }
}

#include "moc_attachmentfrompublickeyjob.cpp"
