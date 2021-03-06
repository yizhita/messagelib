/*
   Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

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

#ifndef MIMETREEPARSER_MESSAGEPART_H
#define MIMETREEPARSER_MESSAGEPART_H

#include "mimetreeparser_export.h"

#include "mimetreeparser/bodypartformatter.h"
#include "mimetreeparser/util.h"

#include <KMime/Message>

#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/importresult.h>
#include <gpgme++/key.h>

#include <QString>
#include <QSharedPointer>

#include <memory>

class QTextCodec;

namespace GpgME {
class ImportResult;
}

namespace QGpgME {
class Protocol;
}

namespace KMime {
class Content;
}

namespace MimeTreeParser {
class CryptoBodyPartMemento;
class MessagePartPrivate;
namespace Interface {
class ObjectTreeSource;
}

class MIMETREEPARSER_EXPORT MessagePart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString plaintextContent READ plaintextContent)
    Q_PROPERTY(QString htmlContent READ htmlContent)
    Q_PROPERTY(bool isAttachment READ isAttachment)
    Q_PROPERTY(bool root READ isRoot)
    Q_PROPERTY(bool isHtml READ isHtml)
    Q_PROPERTY(bool isImage READ isImage CONSTANT)
    Q_PROPERTY(bool neverDisplayInline READ neverDisplayInline CONSTANT)
    Q_PROPERTY(QString attachmentIndex READ attachmentIndex CONSTANT)
    Q_PROPERTY(QString link READ attachmentLink CONSTANT)
public:
    typedef QSharedPointer<MessagePart> Ptr;
    MessagePart(ObjectTreeParser *otp, const QString &text);
    ~MessagePart();

    void setParentPart(MessagePart *parentPart);
    MessagePart *parentPart() const;

    virtual QString text() const;
    void setText(const QString &text);

    virtual QString plaintextContent() const;
    virtual QString htmlContent() const;

    /** The KMime::Content* node that's represented by this part.
     *  Can be @c nullptr, e.g. for sub-parts of an inline signed body part.
     */
    KMime::Content *content() const;
    void setContent(KMime::Content *node);

    /** The KMime::Content* node that's the source of this part.
     *  This is not necessarily the same as content(), for example for
     *  broken-up multipart nodes.
     */
    KMime::Content *attachmentContent() const;
    void setAttachmentContent(KMime::Content *node);
    bool isAttachment() const;
    /** @see KMime::Content::index() */
    QString attachmentIndex() const;
    /** @see NodeHelper::asHREF */
    QString attachmentLink() const;

    /** Returns a string representation of an URL that can be used
     *  to invoke a BodyPartURLHandler for this body part.
     */
    QString makeLink(const QString &path) const;

    void setIsRoot(bool root);
    bool isRoot() const;

    virtual bool isHtml() const;

    bool neverDisplayInline() const;
    void setNeverDisplayInline(bool displayInline);
    bool isImage() const;
    void setIsImage(bool image);

    PartMetaData *partMetaData() const;

    Interface::BodyPartMemento *memento() const;
    void setMemento(Interface::BodyPartMemento *memento);

    /* only a function that should be removed if the refactoring is over */
    virtual void fix() const;

    void appendSubPart(const MessagePart::Ptr &messagePart);
    const QVector<MessagePart::Ptr> &subParts() const;
    bool hasSubParts() const;
    void clearSubParts();

    Interface::ObjectTreeSource *source() const;
    NodeHelper *nodeHelper() const;

protected:
    void parseInternal(KMime::Content *node, bool onlyOneMimePart);
    QString renderInternalText() const;

    ObjectTreeParser *mOtp = nullptr;

private:
    std::unique_ptr<MessagePartPrivate> d;
};

class MIMETREEPARSER_EXPORT MimeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MimeMessagePart> Ptr;
    MimeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart);
    ~MimeMessagePart() override;

    QString text() const override;

    QString plaintextContent() const override;
    QString htmlContent() const override;
private:
    bool mOnlyOneMimePart;
};

class MIMETREEPARSER_EXPORT MessagePartList : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MessagePartList> Ptr;
    explicit MessagePartList(MimeTreeParser::ObjectTreeParser *otp);
    ~MessagePartList() override;

    QString text() const override;

    QString plaintextContent() const override;
    QString htmlContent() const override;
};

enum IconType {
    NoIcon = 0,
    IconExternal,
    IconInline
};

class MIMETREEPARSER_EXPORT TextMessagePart : public MessagePartList
{
    Q_OBJECT
    Q_PROPERTY(bool showLink READ showLink CONSTANT)
    Q_PROPERTY(bool isFirstTextPart READ isFirstTextPart CONSTANT)
    Q_PROPERTY(bool hasLabel READ hasLabel CONSTANT)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
public:
    typedef QSharedPointer<TextMessagePart> Ptr;
    TextMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage);
    ~TextMessagePart() override;

    KMMsgSignatureState signatureState() const;
    KMMsgEncryptionState encryptionState() const;

    bool decryptMessage() const;

    bool showLink() const;
    bool isFirstTextPart() const;
    bool hasLabel() const;

    /** The attachment filename, or the closest approximation thereof we have. */
    QString label() const;
    /** A description of this attachment, if provided. */
    QString comment() const;
    /** Temporary file containing the part content. */
    QString temporaryFilePath() const;

private:
    void parseContent();

    KMMsgSignatureState mSignatureState;
    KMMsgEncryptionState mEncryptionState;
    bool mDecryptMessage;
};

class MIMETREEPARSER_EXPORT AttachmentMessagePart : public TextMessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AttachmentMessagePart> Ptr;
    AttachmentMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage);
    ~AttachmentMessagePart() override;
};

class MIMETREEPARSER_EXPORT HtmlMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<HtmlMessagePart> Ptr;
    HtmlMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, MimeTreeParser::Interface::ObjectTreeSource *source);
    ~HtmlMessagePart() override;

    QString text() const override;

    void fix() const override;
    bool isHtml() const override;

    QString bodyHtml() const;

private:
    Interface::ObjectTreeSource *mSource;
    QString mBodyHTML;
    QByteArray mCharset;
};

class MIMETREEPARSER_EXPORT AlternativeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AlternativeMessagePart> Ptr;
    AlternativeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, Util::HtmlMode preferredMode);
    ~AlternativeMessagePart() override;

    QString text() const override;

    Util::HtmlMode preferredMode() const;
    void setPreferredMode(Util::HtmlMode preferredMode);

    bool isHtml() const override;

    QString plaintextContent() const override;
    QString htmlContent() const override;

    QList<Util::HtmlMode> availableModes();

    void fix() const override;

    const QMap<Util::HtmlMode, MimeMessagePart::Ptr> &childParts() const;

private:
    Util::HtmlMode mPreferredMode;

    QMap<Util::HtmlMode, KMime::Content *> mChildNodes;
    QMap<Util::HtmlMode, MimeMessagePart::Ptr> mChildParts;
};

class MIMETREEPARSER_EXPORT CertMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<CertMessagePart> Ptr;
    CertMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const QGpgME::Protocol *cryptoProto, bool autoImport);
    ~CertMessagePart() override;

    QString text() const override;

    const GpgME::ImportResult &importResult() const;

private:
    bool mAutoImport;
    GpgME::ImportResult mImportResult;
    const QGpgME::Protocol *mCryptoProto;
};

class MIMETREEPARSER_EXPORT EncapsulatedRfc822MessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<EncapsulatedRfc822MessagePart> Ptr;
    EncapsulatedRfc822MessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message);
    ~EncapsulatedRfc822MessagePart() override;

    QString text() const override;

    void fix() const override;

    const KMime::Message::Ptr message() const;

private:
    const KMime::Message::Ptr mMessage;
};

class MIMETREEPARSER_EXPORT EncryptedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool decryptMessage READ decryptMessage WRITE setDecryptMessage)
    Q_PROPERTY(bool isEncrypted READ isEncrypted)
    Q_PROPERTY(bool isNoSecKey READ isNoSecKey)
    Q_PROPERTY(bool passphraseError READ passphraseError)
public:
    typedef QSharedPointer<EncryptedMessagePart> Ptr;
    EncryptedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    ~EncryptedMessagePart() override;

    QString text() const override;

    void setDecryptMessage(bool decrypt);
    bool decryptMessage() const;

    void setIsEncrypted(bool encrypted);
    bool isEncrypted() const;

    bool isDecryptable() const;

    bool isNoSecKey() const;
    bool passphraseError() const;

    void startDecryption(const QByteArray &text, const QTextCodec *aCodec);
    void startDecryption(KMime::Content *data = nullptr);

    QByteArray mDecryptedData;

    QString plaintextContent() const override;
    QString htmlContent() const override;

    const QGpgME::Protocol *cryptoProto() const;
    QString fromAddress() const;

    const std::vector<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> > &decryptRecipients() const;

private:
    /** Handles the decryption of a given content
     * returns true if the decryption was successful
     * if used in async mode, check if mMetaData.inPogress is true, it initiates a running decryption process.
     */
    bool okDecryptMIME(KMime::Content &data);

protected:
    bool mPassphraseError;
    bool mNoSecKey;
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    bool mDecryptMessage;
    QByteArray mVerifiedText;
    std::vector<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> > mDecryptRecipients;

    friend class EncryptedBodyPartFormatter;
};

class MIMETREEPARSER_EXPORT SignedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool isSigned READ isSigned)
public:
    typedef QSharedPointer<SignedMessagePart> Ptr;
    SignedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    ~SignedMessagePart() override;

    void setIsSigned(bool isSigned);
    bool isSigned() const;

    void startVerification(const QByteArray &text, const QTextCodec *aCodec);
    void startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature);

    QByteArray mDecryptedData;
    std::vector<GpgME::Signature> mSignatures;

    QString plaintextContent() const override;
    QString htmlContent() const override;

    const QGpgME::Protocol *cryptoProto() const;
    QString fromAddress() const;

private:
    /** Handles the verification of data
     * If signature is empty it is handled as inline signature otherwise as detached signature mode.
     * Returns true if the verification was successful and the block is signed.
     * If used in async mode, check if mMetaData.inProgress is true, it initiates a running verification process.
     */
    bool okVerify(const QByteArray &data, const QByteArray &signature, KMime::Content *textNode);

    void sigStatusToMetaData();

    void setVerificationResult(const CryptoBodyPartMemento *m, KMime::Content *textNode);
protected:
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    QByteArray mVerifiedText;

    friend EncryptedMessagePart;
};
}

#endif //__MIMETREEPARSER_MESSAGEPART_H
