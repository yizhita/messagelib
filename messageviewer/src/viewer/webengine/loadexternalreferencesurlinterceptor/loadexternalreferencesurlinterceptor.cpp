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

#include "loadexternalreferencesurlinterceptor.h"

#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;

LoadExternalReferencesUrlInterceptor::LoadExternalReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

LoadExternalReferencesUrlInterceptor::~LoadExternalReferencesUrlInterceptor()
{
}

bool LoadExternalReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QString scheme = info.requestUrl().scheme();
    if (scheme == QStringLiteral("data")
        || scheme == QStringLiteral("file")) {
        return false;
    }
    if (mAllowLoadExternalReference) {
        return false;
    } else {
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage
            && !info.requestUrl().isLocalFile()
            && (scheme != QLatin1String("cid"))) {
            return true;
        }
    }
    return false;
}

void LoadExternalReferencesUrlInterceptor::setAllowExternalContent(bool b)
{
    mAllowLoadExternalReference = b;
}

bool LoadExternalReferencesUrlInterceptor::allowExternalContent() const
{
    return mAllowLoadExternalReference;
}
