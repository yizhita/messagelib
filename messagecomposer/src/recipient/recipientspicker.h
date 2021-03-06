/*
    Copyright (c) 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    Copyright (c) 2005 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef MESSAGECOMPOSER_RECIPIENTSPICKER_H
#define MESSAGECOMPOSER_RECIPIENTSPICKER_H

#include <MessageComposer/Recipient>

#include <kcontacts/addressee.h>
#include <QDialog>

class QPushButton;

namespace KLDAP {
class LdapSearchDialog;
}

namespace MessageComposer {
class RecipientsPickerWidget;
class RecipientsPicker : public QDialog
{
    Q_OBJECT

public:
    explicit RecipientsPicker(QWidget *parent);
    ~RecipientsPicker() override;

    void setRecipients(const Recipient::List &);

    void setDefaultType(Recipient::Type);

Q_SIGNALS:
    void pickedRecipient(const Recipient &, bool &);

protected:
    void readConfig();
    void writeConfig();

    void pick(Recipient::Type);

    void keyPressEvent(QKeyEvent *) override;

protected Q_SLOTS:
    void slotToClicked();
    void slotCcClicked();
    void slotBccClicked();
    void slotReplyToClicked();
    void slotPicked();
    void slotSearchLDAP();
    void ldapSearchResult();
    void slotSelectionChanged();

private:
    MessageComposer::RecipientsPickerWidget *mView = nullptr;

    KLDAP::LdapSearchDialog *mLdapSearchDialog = nullptr;

    Recipient::Type mDefaultType;
    QPushButton *mUser4Button = nullptr;
    QPushButton *mUser3Button = nullptr;
    QPushButton *mUser2Button = nullptr;
    QPushButton *mUser1Button = nullptr;
};
}

#endif
