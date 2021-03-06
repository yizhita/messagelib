/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef MESSAGECORE_MAILINGLIST_H
#define MESSAGECORE_MAILINGLIST_H

#include "messagecore_export.h"

#include <kmime/kmime_message.h>
#include <QUrl>

#include <QByteArray>
#include <QSharedDataPointer>
#include <QString>

class KConfigGroup;

namespace MessageCore {
/**
 * @short A class to extract information about mailing lists from emails.
 *
 * The mailing list header fields are defined as the following:
 *   - "List-*" in RFC2369
 *   - "List-ID" in RFC2919.
 *   - "Archive-At" in RFC5064
 *
 * @author Zack Rusin <zack@kde.org>
 */
class MESSAGECORE_EXPORT MailingList
{
public:
    /**
     * Defines what entity should manage the mailing list.
     */
    enum Handler {
        KMail,   ///< The list is handled by KMail
        Browser  ///< The list is handled by a browser.
    };

    /**
     * Defines the features a mailinglist can support.
     */
    enum Feature {
        None = 0 << 0,         ///< No mailing list fields exist.
        Post = 1 << 0,         ///< List-Post header exists.
        Subscribe = 1 << 1,    ///< List-Subscribe header exists.
        Unsubscribe = 1 << 2,  ///< List-Unsubscribe header exists.
        Help = 1 << 3,         ///< List-Help header exists.
        Archive = 1 << 4,      ///< List-Archive header exists.
        Id = 1 << 5,           ///< List-ID header exists.
        Owner = 1 << 6,        ///< List-Owner header exists.
        ArchivedAt = 1 << 7    ///< Archive-At header exists.
    };
    Q_DECLARE_FLAGS(Features, Feature)

public:
    /**
     * Extracts the information about a mailing list from the given @p message.
     */
    static MailingList detect(const KMime::Message::Ptr &message);

    static QString name(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue);

public:
    /**
     * Creates an empty mailing list.
     */
    MailingList();

    /**
     * Creates a mailing list from an @p other mailing list.
     */
    MailingList(const MailingList &other);

    /**
     * Overwrites this mailing list with an @p other mailing list.
     */
    MailingList &operator=(const MailingList &other);

    Q_REQUIRED_RESULT bool operator==(const MailingList &other) const;
    /**
     * Destroys the mailing list.
     */
    ~MailingList();

    /**
     * Returns the features the mailing list supports.
     */
    Q_REQUIRED_RESULT Features features() const;

    /**
     * Sets the @p handler for the mailing list.
     */
    void setHandler(Handler handler);

    /**
     * Returns the handler for the mailing list.
     */
    Q_REQUIRED_RESULT Handler handler() const;

    /**
     * Sets the list of List-Post @p urls.
     */
    void setPostUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Post urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> postUrls() const;

    /**
     * Sets the list of List-Subscribe @p urls.
     */
    void setSubscribeUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Subscribe urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> subscribeUrls() const;

    /**
     * Sets the list of List-Unsubscribe @p urls.
     */
    void setUnsubscribeUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Unsubscribe urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> unsubscribeUrls() const;

    /**
     * Sets the list of List-Help @p urls.
     */
    void setHelpUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Help urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> helpUrls() const;

    /**
     * Sets the list of List-Archive @p urls.
     */
    void setArchiveUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Archive urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> archiveUrls() const;

    /**
     * Sets the list of List-Owner @p urls.
     */
    void setOwnerUrls(const QList<QUrl> &urls);

    /**
     * Returns the list of List-Owner urls.
     */
    Q_REQUIRED_RESULT QList<QUrl> ownerUrls() const;

    /**
     * Sets the Archived-At @p url.
     */
    void setArchivedAtUrls(const QList<QUrl> &url);

    /**
     * Returns the Archived-At @p url.
     */
    Q_REQUIRED_RESULT QList<QUrl> archivedAtUrls() const;

    /**
     * Sets the @p id of the mailing list.
     */
    void setId(const QString &id);

    /**
     * Returns the @p id of the mailing list.
     */
    Q_REQUIRED_RESULT QString id() const;

    /**
     * Saves the configuration for the mailing list to the config @p group.
     */
    void writeConfig(KConfigGroup &group) const;

    /**
     * Restores the configuration for the mailing list from the config @p group.
     */
    void readConfig(const KConfigGroup &group);

private:
    class Private;
    QSharedDataPointer<Private> d;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(MessageCore::MailingList::Features)
Q_DECLARE_METATYPE(MessageCore::MailingList::Features)

#endif
