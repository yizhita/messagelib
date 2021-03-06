/*
   Copyright (C) 2014-2019 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef FOLLOWUPREMINDERCREATEJOB_H
#define FOLLOWUPREMINDERCREATEJOB_H

#include <QObject>
#include <QDate>
#include <AkonadiCore/Item>
#include <AkonadiCore/Collection>
#include "FollowupReminder/FollowUpReminderInfo"
#include <KJob>
#include "messagecomposer_export.h"
namespace MessageComposer {
class FollowupReminderCreateJobPrivate;
class MESSAGECOMPOSER_EXPORT FollowupReminderCreateJob : public KJob
{
    Q_OBJECT
public:
    explicit FollowupReminderCreateJob(QObject *parent = nullptr);
    ~FollowupReminderCreateJob() override;

    void setFollowUpReminderDate(const QDate &date);

    void setOriginalMessageItemId(Akonadi::Item::Id value);

    void setMessageId(const QString &messageId);

    void setTo(const QString &to);

    void setSubject(const QString &subject);

    void setCollectionToDo(const Akonadi::Collection &collection);

    void start() override;

private Q_SLOTS:
    void slotCreateNewTodo(KJob *job);
private:
    void writeFollowupReminderInfo();
    FollowupReminderCreateJobPrivate *const d;
};
}

#endif // FOLLOWUPREMINDERCREATEJOB_H
