/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MAILNETWORKURLINTERCEPTORMANAGER_H
#define MAILNETWORKURLINTERCEPTORMANAGER_H

#include <QObject>
#include "networkpluginurlinterceptor.h"
#include "networkpluginurlinterceptorinterface.h"
#include "webengineviewer_export.h"
#include <QVector>
#include <WebEngineViewer/WebHitTestResult>
class KActionCollection;
class QWebEngineView;
namespace WebEngineViewer {
class WebHitTestResult;
class NetworkUrlInterceptorManagerPrivate;
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUrlInterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent = nullptr);
    ~NetworkUrlInterceptorManager();

    Q_REQUIRED_RESULT QVector<NetworkPluginUrlInterceptorInterface *> interfaceList() const;
    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
private:
    NetworkUrlInterceptorManagerPrivate *const d;
};
}
#endif // MAILNETWORKURLINTERCEPTORMANAGER_H
