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

#ifndef MAILNETWORKPLUGINURLINTERCEPTOR_H
#define MAILNETWORKPLUGINURLINTERCEPTOR_H

#include <QObject>
#include "webengineviewer_export.h"
class QWebEngineView;
namespace WebEngineViewer
{
class NetworkPluginUrlInterceptorConfigureWidget;
class NetworkPluginUrlInterceptorInterface;
struct WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorConfigureWidgetSetting {
    NetworkPluginUrlInterceptorConfigureWidgetSetting()
        : configureWidget(Q_NULLPTR)
    {

    }

    NetworkPluginUrlInterceptorConfigureWidgetSetting(WebEngineViewer::NetworkPluginUrlInterceptorConfigureWidget *w, const QString &widgetName)
        : name(widgetName),
          configureWidget(w)
    {

    }

    QString name;
    WebEngineViewer::NetworkPluginUrlInterceptorConfigureWidget *configureWidget;
};

class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptor(QObject *parent = Q_NULLPTR);
    ~NetworkPluginUrlInterceptor();
    virtual NetworkPluginUrlInterceptorInterface *createInterface(QWebEngineView *webEngine, QObject *parent = Q_NULLPTR) = 0;

    virtual bool hasConfigureSupport() const;
    virtual NetworkPluginUrlInterceptorConfigureWidgetSetting createConfigureWidget(QWidget *parent = Q_NULLPTR);
};
}

Q_DECLARE_TYPEINFO(WebEngineViewer::NetworkPluginUrlInterceptorConfigureWidgetSetting, Q_MOVABLE_TYPE);
#endif // MAILNETWORKPLUGINURLINTERCEPTOR_H
