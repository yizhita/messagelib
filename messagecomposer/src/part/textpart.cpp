/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

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

#include "textpart.h"

using namespace MessageComposer;

class Q_DECL_HIDDEN TextPart::Private
{
public:
    Private()
    {
    }

    KPIMTextEdit::ImageList embeddedImages;
    QString cleanPlainText;
    QString wrappedPlainText;
    QString cleanHtml;
    bool wordWrappingEnabled = true;
    bool warnBadCharset = true;
};

TextPart::TextPart(QObject *parent)
    : MessagePart(parent)
    , d(new Private)
{
}

TextPart::~TextPart()
{
    delete d;
}

bool TextPart::isWordWrappingEnabled() const
{
    return d->wordWrappingEnabled;
}

void TextPart::setWordWrappingEnabled(bool enabled)
{
    d->wordWrappingEnabled = enabled;
}

bool TextPart::warnBadCharset() const
{
    return d->warnBadCharset;
}

void TextPart::setWarnBadCharset(bool warn)
{
    d->warnBadCharset = warn;
}

QString TextPart::cleanPlainText() const
{
    return d->cleanPlainText;
}

void TextPart::setCleanPlainText(const QString &text)
{
    d->cleanPlainText = text;
}

QString TextPart::wrappedPlainText() const
{
    return d->wrappedPlainText;
}

void TextPart::setWrappedPlainText(const QString &text)
{
    d->wrappedPlainText = text;
}

bool TextPart::isHtmlUsed() const
{
    return !d->cleanHtml.isEmpty();
}

QString TextPart::cleanHtml() const
{
    return d->cleanHtml;
}

void TextPart::setCleanHtml(const QString &text)
{
    d->cleanHtml = text;
}

bool TextPart::hasEmbeddedImages() const
{
    return !d->embeddedImages.isEmpty();
}

KPIMTextEdit::ImageList TextPart::embeddedImages() const
{
    return d->embeddedImages;
}

void TextPart::setEmbeddedImages(const KPIMTextEdit::ImageList &images)
{
    d->embeddedImages = images;
}
