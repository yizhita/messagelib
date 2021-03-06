/*  -*- mode: C++; c-file-style: "gnu" -*-
    bodypartformatter.h

    This file is part of KMail's plugin interface.
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

#ifndef MIMETREEPARSER_INTERFACE_BODYPARTFORMATTER_H
#define MIMETREEPARSER_INTERFACE_BODYPARTFORMATTER_H

#include "mimetreeparser_export.h"

#include <QSharedPointer>

#include "mimetreeparser/objecttreeparser.h"

namespace MimeTreeParser {
class MessagePart;
typedef QSharedPointer<MessagePart> MessagePartPtr;

namespace Interface {
class BodyPart;

class MIMETREEPARSER_EXPORT BodyPartFormatter
{
public:
    virtual ~BodyPartFormatter();

    virtual MimeTreeParser::MessagePartPtr process(BodyPart &part) const = 0;
};

/**
    @short interface for BodyPartFormatter plugins

    The interface is queried by for types, subtypes, and the
    corresponding bodypart formatter, and the result inserted into
    the bodypart formatter factory.

    Subtype alone or both type and subtype may be "*", which is
    taken as a wildcard, so that e.g. type=text subtype=* matches
    any text subtype, but with lesser specificity than a concrete
    mimetype such as text/plain. type=* is only allowed when
    subtype=*, too.
*/
class MIMETREEPARSER_EXPORT BodyPartFormatterPlugin
{
public:
    virtual ~BodyPartFormatterPlugin();

    virtual const BodyPartFormatter *bodyPartFormatter(int idx) const = 0;
};
} // namespace Interface
}

Q_DECLARE_INTERFACE(MimeTreeParser::Interface::BodyPartFormatterPlugin, "org.kde.messageviewer.bodypartformatter/1.1")

#endif // MIMETREEPARSER_INTERFACE_BODYPARTFORMATTER_H
