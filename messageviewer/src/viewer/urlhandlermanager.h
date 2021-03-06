/*  -*- c++ -*-
    urlhandlermanager.h

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>
    Copyright (C) 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>

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

#ifndef MESSAGEVIEWER_URLHANDLERMANAGER_H
#define MESSAGEVIEWER_URLHANDLERMANAGER_H

#include "messageviewer_private_export.h"

#include <QVector>

class QUrl;
class QString;
class QPoint;

namespace MimeTreeParser {
class URLHandler;
}

namespace MessageViewer {
namespace Interface {
class BodyPartURLHandler;
}
class ViewerPrivate;
class BodyPartURLHandlerManager;
/**
  * @short Singleton to manage the list of URLHandlers
  * @author Marc Mutz <mutz@kde.org>
  */
class MESSAGEVIEWER_TESTS_EXPORT URLHandlerManager
{
    static URLHandlerManager *self;

    URLHandlerManager();
public:
    ~URLHandlerManager();

    static URLHandlerManager *instance();

    void registerHandler(const MimeTreeParser::URLHandler *handler);
    void unregisterHandler(const MimeTreeParser::URLHandler *handler);

    void registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType);
    void unregisterHandler(const Interface::BodyPartURLHandler *handler);

    Q_REQUIRED_RESULT bool handleClick(const QUrl &url, ViewerPrivate *w = nullptr) const;
    Q_REQUIRED_RESULT bool handleShiftClick(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT bool handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w = nullptr) const;
    Q_REQUIRED_RESULT bool willHandleDrag(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT bool handleDrag(const QUrl &url, ViewerPrivate *window = nullptr) const;
    Q_REQUIRED_RESULT QString statusBarMessage(const QUrl &url, ViewerPrivate *w = nullptr) const;

private:
    typedef QVector<const MimeTreeParser::URLHandler *> HandlerList;
    HandlerList mHandlers;
    BodyPartURLHandlerManager *mBodyPartURLHandlerManager = nullptr;
};
}

#endif // MESSAGEVIEWER_URLHANDLERMANAGER_H
