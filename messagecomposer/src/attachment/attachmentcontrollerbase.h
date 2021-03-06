/*
 * This file is part of KMail.
 * Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
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

#ifndef KMAIL_ATTACHMENTCONTROLLERBASE_H
#define KMAIL_ATTACHMENTCONTROLLERBASE_H

#include <QObject>

#include <QUrl>

#include <MessageCore/AttachmentPart>
#include <MessageViewer/EditorWatcher>
#include <KJob>
#include <KService>
#include "messagecomposer_export.h"

class KActionCollection;
class QAction;

namespace MessageComposer {
class AttachmentModel;
class MESSAGECOMPOSER_EXPORT AttachmentControllerBase : public QObject
{
    Q_OBJECT

public:
    AttachmentControllerBase(MessageComposer::AttachmentModel *model, QWidget *wParent, KActionCollection *actionCollection);
    ~AttachmentControllerBase();

    void createActions();

    // TODO dnd stuff...

    void setSelectedParts(const MessageCore::AttachmentPart::List &selectedParts);

    void setAttachOwnVcard(bool attachVcard);
    Q_REQUIRED_RESULT bool attachOwnVcard() const;
    void setIdentityHasOwnVcard(bool state);

public Q_SLOTS:
    /// model sets these
    void setEncryptEnabled(bool enabled);
    void setSignEnabled(bool enabled);
    /// compression is async...
    void compressAttachment(const MessageCore::AttachmentPart::Ptr &part, bool compress);
    void showContextMenu();
    void openAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void viewAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void editAttachment(MessageCore::AttachmentPart::Ptr part, MessageViewer::EditorWatcher::OpenWithOption option = MessageViewer::EditorWatcher::NoOpenWithDialog);
    void editAttachmentWith(const MessageCore::AttachmentPart::Ptr &part);
    void saveAttachmentAs(const MessageCore::AttachmentPart::Ptr &part);
    void attachmentProperties(const MessageCore::AttachmentPart::Ptr &part);
    void showAddAttachmentFileDialog();
    void showAddAttachmentCompressedDirectoryDialog();
    /// sets sign, encrypt, shows properties dialog if so configured
    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void addAttachment(const QUrl &url);
    void addAttachmentUrlSync(const QUrl &url);
    void addAttachments(const QList<QUrl> &urls);
    void showAttachPublicKeyDialog();
    void showAttachVcard();
    void showAttachClipBoard();
    virtual void attachMyPublicKey();

Q_SIGNALS:
    void actionsCreated();
    void refreshSelection();
    void showAttachment(KMime::Content *content, const QByteArray &charset);
    void selectedAllAttachment();
    void addOwnVcard(bool);
    void fileAttached();

protected:
    void exportPublicKey(const QString &fingerprint);
    void enableAttachPublicKey(bool enable);
    void enableAttachMyPublicKey(bool enable);
    void byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite = false);
    void openWith(const KService::Ptr &offer = KService::Ptr());

private:
    void attachFiles(const QList<QUrl> &urls, const QString &encoding);
    void attachDirectory(const QUrl &url);
    void slotPutResult(KJob *job);
    void slotOpenWithDialog();
    void slotOpenWithAction(QAction *act);

private:
    class Private;
    Private *const d;
};
} //

#endif // KMAIL_ATTACHMENTCONTROLLERBASE_H
