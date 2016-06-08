/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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
#include "networkaccessmanagerwebengine.h"
#include "urlinterceptor/networkurlinterceptormanager.h"
#include "urlinterceptor/networkurlinterceptor.h"
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate()
        : mManager(Q_NULLPTR),
          mNetworkUrlInterceptor(Q_NULLPTR)
    {

    }
    WebEngineViewer::NetworkUrlInterceptorManager *mManager;
    WebEngineViewer::NetworkUrlInterceptor *mNetworkUrlInterceptor;
};

NetworkAccessManagerWebEngine::NetworkAccessManagerWebEngine(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QNetworkAccessManager(parent),
      d(new NetworkAccessManagerWebEnginePrivate)
{
    d->mManager = new WebEngineViewer::NetworkUrlInterceptorManager(webEngine, ac, this);

    // Add interceptor.
    d->mNetworkUrlInterceptor = new WebEngineViewer::NetworkUrlInterceptor(this);
    Q_FOREACH (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface, d->mManager->interfaceList()) {
        d->mNetworkUrlInterceptor->addInterceptor(interface);
    }
    webEngine->page()->profile()->setRequestInterceptor(d->mNetworkUrlInterceptor);
}

NetworkAccessManagerWebEngine::~NetworkAccessManagerWebEngine()
{
    delete d;
}

void NetworkAccessManagerWebEngine::addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->mNetworkUrlInterceptor->addInterceptor(interceptor);
}

QList<QAction *> NetworkAccessManagerWebEngine::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    QList<QAction *> actions;
    Q_FOREACH (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface, d->mManager->interfaceList()) {
        actions.append(interface->interceptorUrlActions(result));
    }
    return actions;
}

QVector<NetworkPluginUrlInterceptorConfigureWidgetSetting> NetworkAccessManagerWebEngine::configureInterceptorList(QWidget *parent) const
{
    return d->mManager->configureInterceptorList(parent);
}
