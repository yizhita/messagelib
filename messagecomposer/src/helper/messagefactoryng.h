/*
  Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>
  Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#ifndef MESSAGECOMPOSER_MESSAGE_FACTORYNG_H
#define MESSAGECOMPOSER_MESSAGE_FACTORYNG_H

#include "messagecomposer_export.h"
#include <kmime/kmime_message.h>
#include <kmime/kmime_mdn.h>

#include <Item>
#include <Collection>

#include <Akonadi/KMime/MessageStatus>

namespace KIdentityManagement {
class IdentityManager;
}

namespace MessageComposer {
/**
   * Enumeration that defines the available reply "modes"
   */
enum ReplyStrategy {
    ReplySmart = 0,    //< Attempt to automatically guess the best recipient for the reply
    ReplyAuthor,       //< Reply to the author of the message (possibly NOT the mailing list, if any)
    ReplyList,         //< Reply to the mailing list (and not the author of the message)
    ReplyAll,          //< Reply to author and all the recipients in CC
    ReplyNone          //< Don't set reply addresses: they will be set manually
};

enum MDNAdvice {
    MDNIgnore,
    MDNSendDenied,
    MDNSend
};
/**
 * Contains various factory methods for creating new messages such as replies, MDNs, forwards, etc.
 */
class MESSAGECOMPOSER_EXPORT MessageFactoryNG : public QObject
{
    Q_OBJECT
public:

    /// Small helper structure which encapsulates the KMime::Message created when creating a reply, and
    /// the reply mode
    struct MessageReply {
        KMime::Message::Ptr msg;  ///< The actual reply message
        bool replyAll;   ///< If true, the "reply all" template was used, otherwise the normal reply
        ///  template
    };

    explicit MessageFactoryNG(const KMime::Message::Ptr &origMsg, Akonadi::Item::Id id, const Akonadi::Collection &col = Akonadi::Collection(), QObject *parent = nullptr);
    ~MessageFactoryNG() override;

    /**
    * Create a new message that is a reply to this message, filling all
    * required header fields with the proper values. The returned message
    * is not stored in any folder. Marks this message as replied.
    *
    */
    void createReplyAsync();

    /** Create a new message that is a forward of this message, filling all
    required header fields with the proper values. The returned message
    is not stored in any folder. Marks this message as forwarded. */
    void createForwardAsync();

    /**
    * Create a forward from the given list of messages, attaching each
    *  message to be forwarded to the new forwarded message.
    *
    * If no list is passed, use the original message passed in the MessageFactoryNG
    *  constructor.
    */
    Q_REQUIRED_RESULT QPair< KMime::Message::Ptr, QList< KMime::Content * > > createAttachedForward(const Akonadi::Item::List &items = Akonadi::Item::List());

    /** Create a new message that is a redirect to this message, filling all
    required header fields with the proper values. The returned message
    is not stored in any folder. Marks this message as replied.
    Redirects differ from forwards so they are forwarded to some other
    user, mail is not changed and the reply-to field is set to
    the email address of the original sender.
    */
    Q_REQUIRED_RESULT KMime::Message::Ptr createRedirect(const QString &toStr, const QString &ccStr = QString(), const QString &bccStr = QString(), int transportId = -1, const QString &fcc = QString(), int identity = -1);

    Q_REQUIRED_RESULT KMime::Message::Ptr createResend();

    /** Create a new message that is a delivery receipt of this message,
      filling required header fileds with the proper values. The
      returned message is not stored in any folder. */
    Q_REQUIRED_RESULT KMime::Message::Ptr createDeliveryReceipt();

    /** Create a new message that is a MDN for this message, filling all
      required fields with proper values. The returned message is not
      stored in any folder.

      @param a Use AutomaticAction for filtering and ManualAction for
               user-induced events.
      @param d See docs for KMime::MDN::DispositionType
      @param s See docs for KMime::MDN::SendingMode (in KMail, use MDNAdvideDialog to ask the user for this parameter)
      @param m See docs for KMime::MDN::DispositionModifier

      @return The notification message or 0, if none should be sent, as well as the state of the MDN operation.
    **/
    Q_REQUIRED_RESULT KMime::Message::Ptr createMDN(KMime::MDN::ActionMode a, KMime::MDN::DispositionType d, KMime::MDN::SendingMode s, int mdnQuoteOriginal = 0, const QVector<KMime::MDN::DispositionModifier> &m = QVector<KMime::MDN::DispositionModifier>());

    /**
    * Create a new forwarded MIME digest. If the user is trying to forward multiple messages
    *  at once all inline, they can choose to have them be compiled into a single digest
    *  message.
    *
    * This will return a header message and individual message parts to be set as
    *  attachments.
    *
    * @param msgs List of messages to be composed into a digest
    */
    Q_REQUIRED_RESULT QPair< KMime::Message::Ptr, KMime::Content * > createForwardDigestMIME(const Akonadi::Item::List &items);

    /**
    * Set the identity manager to be used when creating messages.
    * Required to be set before create* is called, otherwise the created messages
    *  might have the wrong identity data.
    */
    void setIdentityManager(KIdentityManagement::IdentityManager *ident);

    /**
    * Set the reply strategy to use. Default is ReplySmart.
    */
    void setReplyStrategy(MessageComposer::ReplyStrategy replyStrategy);

    /**
    * Set the selection to be used to  base the reply on.
    */
    void setSelection(const QString &selection);

    /**
    * Whether to quote the original message in the reply.
    *  Default is to quote.
    */
    void setQuote(bool quote);

    /**
    * Set the template to be used when creating the reply. Default is to not
    *  use any template at all.
    */
    void setTemplate(const QString &templ);

    /**
    * Set extra mailinglist addresses to send the created message to.
    * Any mailing-list addresses specified in the original message
    * itself will be added by MessageFactoryNG, so no need to add those manually.
    */
    void setMailingListAddresses(const KMime::Types::Mailbox::List &listAddresses);

    /**
    *  Set the identity that is set for the folder in which the given message is.
    *   It is used as a fallback when finding the identity if it can't be found in
    *   any other way.
    *  Also used if putRepliesInSameFolder is set to true.
    */
    void setFolderIdentity(Akonadi::Item::Id folderIdentityId);

    /**
    * Whether or not to put the reply to a message in the same folder as the message itself.
    *  If so, specify the folder id in which to put them. Default is -1, which means to not put
    *  replies in the same folder at all.
    */
    void putRepliesInSameFolder(Akonadi::Item::Id parentColId = -1);

    /**
    * When creating MDNs, the user needs to be asked for confirmation in specific
    *  cases according to RFC 2298.
    */
    Q_REQUIRED_RESULT static bool MDNRequested(const KMime::Message::Ptr &msg);

    /**
    * If sending an MDN requires confirmation due to multiple addresses.
    *
    * RFC 2298: [ Confirmation from the user SHOULD be obtained (or no
    * MDN sent) ] if there is more than one distinct address in the
    * Disposition-Notification-To header.
    */
    Q_REQUIRED_RESULT static bool MDNConfirmMultipleRecipients(const KMime::Message::Ptr &msg);

    /**
    *
    * If sending an MDN requires confirmation due to discrepancy between MDN
    *  header and Return-Path header.
    *
    * RFC 2298: MDNs SHOULD NOT be sent automatically if the address in
    * the Disposition-Notification-To header differs from the address
    * in the Return-Path header. [...] Confirmation from the user
    * SHOULD be obtained (or no MDN sent) if there is no Return-Path
    * header in the message [...]
    */
    Q_REQUIRED_RESULT static bool MDNReturnPathEmpty(const KMime::Message::Ptr &msg);
    Q_REQUIRED_RESULT static bool MDNReturnPathNotInRecieptTo(const KMime::Message::Ptr &msg);

    /**
    * If the MDN headers contain options that KMail can't parse
    */
    Q_REQUIRED_RESULT static bool MDNMDNUnknownOption(const KMime::Message::Ptr &msg);

Q_SIGNALS:
    void createReplyDone(const MessageComposer::MessageFactoryNG::MessageReply &reply);
    void createForwardDone(const KMime::Message::Ptr &msg);

private Q_SLOTS:
    void slotCreateReplyDone(const KMime::Message::Ptr &msg, bool replyAll);
    void slotCreateForwardDone(const KMime::Message::Ptr &msg);

private:
    /** @return the UOID of the identity for this message.
      Searches the "x-kmail-identity" header and if that fails,
      searches with KIdentityManagement::IdentityManager::identityForAddress()
    **/
    uint identityUoid(const KMime::Message::Ptr &msg);

    QString replaceHeadersInString(const KMime::Message::Ptr &msg, const QString &s);

    /*
    * If force charset option is enabled, try to set the original charset
    *  in the newly created message. If unable to encode, fall back to
    *  preferred charsets, and if all fail, use UTF-8.
    */
    void applyCharset(const KMime::Message::Ptr msg);

    QByteArray getRefStr(const KMime::Message::Ptr &msg);
    KMime::Content *createForwardAttachmentMessage(const KMime::Message::Ptr &fwdMsg);

    KIdentityManagement::IdentityManager *m_identityManager = nullptr;
    // Required parts to create messages
    KMime::Message::Ptr m_origMsg;
    Akonadi::Item::Id m_folderId;
    Akonadi::Item::Id m_parentFolderId;

    Akonadi::Collection m_collection;

    // Optional settings the calling class may set
    MessageComposer::ReplyStrategy m_replyStrategy;
    QString m_selection, m_template;
    bool m_quote = false;
    KMime::Types::Mailbox::List m_mailingListAddresses;
    Akonadi::Item::Id m_id;
};
}

Q_DECLARE_METATYPE(MessageComposer::ReplyStrategy)
Q_DECLARE_METATYPE(MessageComposer::MessageFactoryNG::MessageReply)

#endif
