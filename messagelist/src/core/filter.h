/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef MESSAGELIST_CORE_FILTER_H
#define MESSAGELIST_CORE_FILTER_H

#include <QObject>
#include <QSet>
#include <QString>

#include <Akonadi/KMime/MessageStatus>
#include <Collection>
#include "widgets/quicksearchline.h"

namespace MessageList {
namespace Core {
class MessageItem;

/**
 * This class is responsable of matching messages that should be displayed
 * in the View. It's used mainly by Model and Widget.
 */
class Filter : public QObject
{
    Q_OBJECT

public:
    explicit Filter(QObject *parent = nullptr);

public:
    /**
    * Returns true if the specified parameters match this filter and false otherwise.
    * The msg pointer must not be null.
    */
    Q_REQUIRED_RESULT bool match(const MessageItem *item) const;

    /**
    * Returns the currently set status mask
    */
    Q_REQUIRED_RESULT QList<Akonadi::MessageStatus> status() const
    {
        return mStatus;
    }

    /**
    * Sets the status mask for this filter.
    */
    void setStatus(const QList<Akonadi::MessageStatus> &lstStatus)
    {
        mStatus = lstStatus;
    }

    /**
    * Sets the current folder of this filter.
    */
    void setCurrentFolder(const Akonadi::Collection &collection);

    /**
    * Returns the currently set search string.
    */
    const QString &searchString() const
    {
        return mSearchString;
    }

    /**
    * Sets the search string for this filter.
    */
    void setSearchString(const QString &search, QuickSearchLine::SearchOptions options);

    /**
    * Returns the currently set MessageItem::Tag id
    */
    const QString &tagId() const
    {
        return mTagId;
    }

    /**
    * Sets the id of a MessageItem::Tag that the matching messages must contain.
    */
    void setTagId(const QString &tagId)
    {
        mTagId = tagId;
    }

    /**
    * Clears this filter (sets status to 0, search string and tag id to empty strings)
    */
    void clear();

    /**
    * Returns true if this filter is empty (0 status mask, empty search string and empty tag)
    * and it's useless to call match() that will always return true.
    */
    Q_REQUIRED_RESULT bool isEmpty() const;

    Q_REQUIRED_RESULT QuickSearchLine::SearchOptions currentOptions() const;
Q_SIGNALS:
    void finished();

private:
    bool containString(const QString &searchInString) const;
    QList<Akonadi::MessageStatus> mStatus;    ///< Messages must match these statuses, if non 0
    QString mSearchString;             ///< Messages must match this search string, if not empty
    QString mTagId;                    ///< Messages must have this tag, if not empty. Contains a tag url.
    Akonadi::Collection mCurrentFolder;
    QSet<qint64> mMatchingItemIds;
    QuickSearchLine::SearchOptions mOptions;
    QStringList mSearchList;
};
} // namespace Core
} // namespace MessageList

#endif //!__MESSAGELIST_CORE_FILTER_H
