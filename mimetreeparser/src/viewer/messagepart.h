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

#ifndef __MIMETREEPARSER_MESSAGEPART_H__
#define __MIMETREEPARSER_MESSAGEPART_H__

#include "mimetreeparser_export.h"

#include "mimetreeparser/bodypartformatter.h"
#include "mimetreeparser/util.h"

#include <KMime/Message>

#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/importresult.h>

#include <QString>
#include <QSharedPointer>

#include <memory>

class QTextCodec;
namespace MessageViewer {
class TextMessagePartRenderer;
class AttachmentMessagePartRenderer;
}

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
class ObjectTreeParser;
class HtmlWriter;
class CryptoBodyPartMemento;
class MessagePartPrivate;
class MultiPartAlternativeBodyPartFormatter;
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
    Q_PROPERTY(bool isHidden READ isHidden)
    Q_PROPERTY(QString attachmentIndex READ attachmentIndex CONSTANT)
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

    void setIsRoot(bool root);
    bool isRoot() const;

    virtual bool isHtml() const;
    virtual bool isHidden() const;

    PartMetaData *partMetaData() const;

    /* only a function that should be removed if the refactoring is over */
    virtual void fix() const;

    void appendSubPart(const MessagePart::Ptr &messagePart);
    const QVector<MessagePart::Ptr> &subParts() const;
    bool hasSubParts() const;

    Interface::ObjectTreeSource *source() const;

protected:
    void parseInternal(KMime::Content *node, bool onlyOneMimePart);
    QString renderInternalText() const;

    ObjectTreeParser *mOtp = nullptr;

private:
    std::unique_ptr<MessagePartPrivate> d;
};

// TODO remove once all plugins are ported away from BPF::format()
class MIMETREEPARSER_DEPRECATED_EXPORT LegacyPluginMessagePart : public MessagePart
{
    Q_OBJECT
public:
    LegacyPluginMessagePart(MimeTreeParser::ObjectTreeParser *otp);
    ~LegacyPluginMessagePart();

    HtmlWriter *htmlWriter() const;
    QString formatOutput() const;

private:
    std::unique_ptr<HtmlWriter> m_htmlWriter;
};

class MIMETREEPARSER_EXPORT MimeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MimeMessagePart> Ptr;
    MimeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart);
    virtual ~MimeMessagePart();

    QString text() const override;

    QString plaintextContent() const override;
    QString htmlContent() const override;
private:
    bool mOnlyOneMimePart;

    friend class AlternativeMessagePart;
};

class MIMETREEPARSER_EXPORT MessagePartList : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MessagePartList> Ptr;
    MessagePartList(MimeTreeParser::ObjectTreeParser *otp);
    virtual ~MessagePartList();

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
    Q_PROPERTY(bool showTextFrame READ showTextFrame CONSTANT)
    Q_PROPERTY(bool showLink READ showLink CONSTANT)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
public:
    typedef QSharedPointer<TextMessagePart> Ptr;
    TextMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage);
    virtual ~TextMessagePart();

    KMMsgSignatureState signatureState() const;
    KMMsgEncryptionState encryptionState() const;

    bool decryptMessage() const;

    bool isHidden() const override;

    bool showLink() const;
    bool showTextFrame() const;

    /** The attachment filename, or the closest approximation thereof we have. */
    QString label() const;
    /** A description of this attachment, if provided. */
    QString comment() const;

private:
    void parseContent();

    KMMsgSignatureState mSignatureState;
    KMMsgEncryptionState mEncryptionState;
    bool mDrawFrame;
    bool mShowLink;
    bool mDecryptMessage;
    bool mIsHidden;

    friend class MessageViewer::TextMessagePartRenderer;
    friend class MessageViewer::AttachmentMessagePartRenderer;
    friend class ObjectTreeParser;
};

class MIMETREEPARSER_EXPORT AttachmentMessagePart : public TextMessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AttachmentMessagePart> Ptr;
    AttachmentMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage);
    virtual ~AttachmentMessagePart();

    IconType asIcon() const;
    bool neverDisplayInline() const;
    void setNeverDisplayInline(bool displayInline);
    bool isImage() const;
    void setIsImage(bool image);

    bool isHidden() const override;

private:
    bool mIsImage;
    bool mNeverDisplayInline;
};

class MIMETREEPARSER_EXPORT HtmlMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<HtmlMessagePart> Ptr;
    HtmlMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, MimeTreeParser::Interface::ObjectTreeSource *source);
    virtual ~HtmlMessagePart();

    QString text() const override;

    void fix() const override;
    bool isHtml() const override;

private:
    Interface::ObjectTreeSource *mSource;
    QString mBodyHTML;
    QByteArray mCharset;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT AlternativeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AlternativeMessagePart> Ptr;
    AlternativeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, Util::HtmlMode preferredMode);
    virtual ~AlternativeMessagePart();

    QString text() const override;

    Util::HtmlMode preferredMode() const;

    bool isHtml() const override;

    QString plaintextContent() const override;
    QString htmlContent() const override;

    QList<Util::HtmlMode> availableModes();

    void fix() const override;
private:
    Util::HtmlMode mPreferredMode;

    QMap<Util::HtmlMode, KMime::Content *> mChildNodes;
    QMap<Util::HtmlMode, MimeMessagePart::Ptr> mChildParts;

    friend class DefaultRendererPrivate;
    friend class ObjectTreeParser;
    friend class MultiPartAlternativeBodyPartFormatter;
};

class MIMETREEPARSER_EXPORT CertMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<CertMessagePart> Ptr;
    CertMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const QGpgME::Protocol *cryptoProto, bool autoImport);
    virtual ~CertMessagePart();

    QString text() const override;

private:
    bool mAutoImport;
    GpgME::ImportResult mImportResult;
    const QGpgME::Protocol *mCryptoProto;
    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT EncapsulatedRfc822MessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<EncapsulatedRfc822MessagePart> Ptr;
    EncapsulatedRfc822MessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message);
    virtual ~EncapsulatedRfc822MessagePart();

    QString text() const override;

    void fix() const override;
private:
    const KMime::Message::Ptr mMessage;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT EncryptedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool decryptMessage READ decryptMessage WRITE setDecryptMessage)
    Q_PROPERTY(bool isEncrypted READ isEncrypted)
    Q_PROPERTY(bool passphraseError READ passphraseError)
public:
    typedef QSharedPointer<EncryptedMessagePart> Ptr;
    EncryptedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    virtual ~EncryptedMessagePart();

    QString text() const override;

    void setDecryptMessage(bool decrypt);
    bool decryptMessage() const;

    void setIsEncrypted(bool encrypted);
    bool isEncrypted() const;

    bool isDecryptable() const;

    bool passphraseError() const;

    void startDecryption(const QByteArray &text, const QTextCodec *aCodec);
    void startDecryption(KMime::Content *data = nullptr);

    QByteArray mDecryptedData;

    QString plaintextContent() const override;
    QString htmlContent() const override;

private:
    /** Handles the dectyptioon of a given content
     * returns true if the decryption was successfull
     * if used in async mode, check if mMetaData.inProgress is true, it inicates a running decryption process.
     */
    bool okDecryptMIME(KMime::Content &data);

protected:
    bool mPassphraseError;
    bool mNoSecKey;
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    bool mDecryptMessage;
    QByteArray mVerifiedText;
    std::vector<GpgME::DecryptionResult::Recipient> mDecryptRecipients;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT SignedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool isSigned READ isSigned)
public:
    typedef QSharedPointer<SignedMessagePart> Ptr;
    SignedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    virtual ~SignedMessagePart();

    void setIsSigned(bool isSigned);
    bool isSigned() const;

    void startVerification(const QByteArray &text, const QTextCodec *aCodec);
    void startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature);

    QByteArray mDecryptedData;
    std::vector<GpgME::Signature> mSignatures;

    QString plaintextContent() const override;
    QString htmlContent() const override;

private:
    /** Handles the verification of data
     * If signature is empty it is handled as inline signature otherwise as detached signature mode.
     * Returns true if the verfication was successfull and the block is signed.
     * If used in async mode, check if mMetaData.inProgress is true, it inicates a running verification process.
     */
    bool okVerify(const QByteArray &data, const QByteArray &signature, KMime::Content *textNode);

    void sigStatusToMetaData();

    void setVerificationResult(const CryptoBodyPartMemento *m, KMime::Content *textNode);
protected:
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    QByteArray mVerifiedText;

    friend EncryptedMessagePart;
    friend class DefaultRendererPrivate;
};
}

#endif //__MIMETREEPARSER_MESSAGEPART_H__
