/*
  This file is part of KMail, the KDE mail client.
  Copyright (c) 2009 Martin Koller <kollix@aon.at>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef MESSAGEVIEWERT_ATTACHMENTDIALOG_H
#define MESSAGEVIEWERT_ATTACHMENTDIALOG_H

#include <QObject>
#include <KService>

class QDialog;
class QDialogButtonBox;
namespace MessageViewer {
/**
 * A class which handles the dialog used to present the user a choice what to do
 * with an attachment.
 */
class AttachmentDialog : public QObject
{
    Q_OBJECT

public:
    /// returncodes for exec()
    enum {
        Save = 2,
        Open,
        OpenWith,
        Cancel
    };

    // if @offer is non-null, the "open with <application>" button will also be shown,
    // otherwise only save, open with, cancel
    explicit AttachmentDialog(QWidget *parent, const QString &filenameText, const KService::Ptr &offer, const QString &dontAskAgainName);

    // executes the modal dialog
    int exec();

private:
    void saveClicked();
    void openClicked();
    void openWithClicked();
    QString text, dontAskName;
    QDialog *dialog = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
};
}

#endif
