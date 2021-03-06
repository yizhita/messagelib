/*
  Copyright (c) 2015-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ATTACHMENTVCARDFROMADDRESSBOOKJOB_H
#define ATTACHMENTVCARDFROMADDRESSBOOKJOB_H

#include "MessageCore/AttachmentLoadJob"
#include <AkonadiCore/Item>
#include "messagecomposer_export.h"
namespace MessageComposer {
class AttachmentVcardFromAddressBookJobPrivate;
class MESSAGECOMPOSER_EXPORT AttachmentVcardFromAddressBookJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT
public:
    explicit AttachmentVcardFromAddressBookJob(const Akonadi::Item &item, QObject *parent = nullptr);
    ~AttachmentVcardFromAddressBookJob() override;

protected Q_SLOTS:
    void doStart() override;

private Q_SLOTS:
    void slotExpandGroupResult(KJob *job);

private:
    void invalidContact();
    void addAttachment(const QByteArray &data, const QString &attachmentName);
    AttachmentVcardFromAddressBookJobPrivate *const d;
};
}
#endif // ATTACHMENTVCARDFROMADDRESSBOOKJOB_H
