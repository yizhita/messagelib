/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

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
#ifndef MESSAGEVIEWER_DEFAULTRENDERER_P_H
#define MESSAGEVIEWER_DEFAULTRENDERER_P_H

#include "defaultrenderer.h"
#include "messagepartrendererbase.h"

#include <MimeTreeParser/MessagePart>

namespace MessageViewer {
class AttachmentStrategy;
class MessagePartRendererFactory;
class CSSHelperBase;
class HtmlWriter;

class DefaultRendererPrivate : public RenderContext
{
public:
    DefaultRendererPrivate(CSSHelperBase *cssHelper, const MessagePartRendererFactory *rendererFactory);
    ~DefaultRendererPrivate() override;

    CSSHelperBase *cssHelper() const override;

    MimeTreeParser::Interface::ObjectTreeSource *source() const;

    void renderSubParts(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) override;

    void render(const MimeTreeParser::MessagePartList::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::MimeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::EncapsulatedRfc822MessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::HtmlMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderEncrypted(const MimeTreeParser::EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderSigned(const MimeTreeParser::SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::AlternativeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::CertMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    bool renderWithFactory(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer) override;
    using RenderContext::renderWithFactory;
    void renderFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter);

    bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    bool showEmoticons() const override;
    bool isPrinting() const override;
    bool htmlLoadExternal() const override;
    bool showExpandQuotesMark() const override;
    bool showOnlyOneMimePart() const override;
    bool showSignatureDetails() const override;
    bool showEncryptionDetails() const override;
    int levelQuote() const override;

    MimeTreeParser::MessagePart::Ptr mMsgPart;
    CSSHelperBase *mCSSHelper = nullptr;
    const MessagePartRendererFactory *mRendererFactory = nullptr;
    const AttachmentStrategy *mAttachmentStrategy = nullptr;
    std::function<QString(KMime::Message *)> mCreateMessageHeader;

    int mLevelQuote = 1;

    bool mShowEmoticons = false;
    bool mIsPrinting = false;
    bool mHtmlLoadExternal = false;
    bool mShowExpandQuotesMark = false;
    bool mShowOnlyOneMimePart = false;
    bool mShowSignatureDetails = false;
    bool mShowEncryptionDetails = false;
};
}
#endif
