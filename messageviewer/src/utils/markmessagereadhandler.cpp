/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Tobias Koenig <tokoe@kdab.com>

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

#include "markmessagereadhandler.h"

#include "settings/messageviewersettings.h"

#include <AkonadiCore/itemmodifyjob.h>
#include <AkonadiCore/Session>
#include <Akonadi/KMime/MessageFlags>

#include <QTimer>
using namespace MessageViewer;
Q_GLOBAL_STATIC(Akonadi::Item::List, sListItem)

class Q_DECL_HIDDEN MarkMessageReadHandler::Private
{
public:
    Private(MarkMessageReadHandler *qq)
        : q(qq)
    {
    }

    void handleMessages();

    MarkMessageReadHandler *q;
    Akonadi::Item mItemQueue;
    QTimer mTimer;
};

void MarkMessageReadHandler::Private::handleMessages()
{
    Akonadi::Item item = mItemQueue;

    // mark as read
    item.setFlag(Akonadi::MessageFlags::Seen);

    Akonadi::ItemModifyJob *modifyJob = new Akonadi::ItemModifyJob(item, q->session());
    modifyJob->disableRevisionCheck();
    modifyJob->setIgnorePayload(true);
    sListItem->removeAll(item);
}

MarkMessageReadHandler::MarkMessageReadHandler(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->mTimer.setSingleShot(true);
    connect(&d->mTimer, &QTimer::timeout, this, [this]() {
        d->handleMessages();
    });
}

MarkMessageReadHandler::~MarkMessageReadHandler()
{
    if (d->mTimer.isActive()) {
        d->mTimer.stop();
    }
    delete d;
}

void MarkMessageReadHandler::setItem(const Akonadi::Item &item)
{
    if (sListItem->contains(item) || d->mItemQueue == item
        || item.hasFlag(Akonadi::MessageFlags::Queued)) {
        return;
    }
    d->mTimer.stop();

    sListItem->removeAll(d->mItemQueue);
    d->mItemQueue = item;
    sListItem->append(item);
    if (item.hasFlag(Akonadi::MessageFlags::Seen)) {
        return;
    }

    if (MessageViewer::MessageViewerSettings::self()->delayedMarkAsRead()) {
        const int delayedMarkTime = MessageViewer::MessageViewerSettings::self()->delayedMarkTime();
        if (delayedMarkTime != 0) {
            d->mTimer.start(delayedMarkTime * 1000);
        } else {
            d->handleMessages();
        }
    }
}

#include "moc_markmessagereadhandler.cpp"
