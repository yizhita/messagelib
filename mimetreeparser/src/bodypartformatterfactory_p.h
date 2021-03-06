/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypartformatterfactory.h

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2004 Marc Mutz <mutz@kde.org>,
                       Ingo Kloecker <kloecker@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#ifndef MIMETREEPARSER_BODYPARTFORMATTERFACTORY_P_H
#define MIMETREEPARSER_BODYPARTFORMATTERFACTORY_P_H

#include <QHash>
#include <vector>

namespace MimeTreeParser {
class BodyPartFormatterFactory;

class BodyPartFormatterFactoryPrivate
{
public:
    explicit BodyPartFormatterFactoryPrivate(BodyPartFormatterFactory *factory);
    ~BodyPartFormatterFactoryPrivate() = default;

    void setup();
    void messageviewer_create_builtin_bodypart_formatters();        //defined in bodypartformatter.cpp
    void insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority = 0);
    void appendFormattersForType(const QString &mimeType, QVector<const Interface::BodyPartFormatter *> &formatters);

    BodyPartFormatterFactory *q;
    struct FormatterInfo {
        const Interface::BodyPartFormatter *formatter = nullptr;
        int priority = 0;
    };
    QHash<QString, std::vector<FormatterInfo> > registry;
};
}

#endif // MIMETREEPARSER_BODYPARTFORMATTERFACTORY_P_H
