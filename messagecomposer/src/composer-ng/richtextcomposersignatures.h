/*
   Copyright (C) 2015-2019 Montel Laurent <montel@kde.org>

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

#ifndef RICHTEXTCOMPOSERSIGNATURES_H
#define RICHTEXTCOMPOSERSIGNATURES_H

#include <QObject>
#include "messagecomposer_export.h"
namespace KIdentityManagement {
class Signature;
}

namespace MessageComposer {
class RichTextComposerNg;
class MESSAGECOMPOSER_EXPORT RichTextComposerSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerSignatures(MessageComposer::RichTextComposerNg *composer, QObject *parent = nullptr);
    ~RichTextComposerSignatures();

    void cleanWhitespace(const KIdentityManagement::Signature &sig);

    Q_REQUIRED_RESULT bool replaceSignature(const KIdentityManagement::Signature &oldSig, const KIdentityManagement::Signature &newSig);
private:
    class RichTextComposerSignaturesPrivate;
    RichTextComposerSignaturesPrivate *const d;
};
}
#endif // RICHTEXTCOMPOSERSIGNATURES_H
