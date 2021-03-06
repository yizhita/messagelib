/*
   Copyright (C) 2015-2019 Laurent Montel <montel@kde.org>

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

#ifndef HEADERSTYLEPLUGIN_H
#define HEADERSTYLEPLUGIN_H

#include <QObject>

#include "messageviewer_export.h"
class KActionCollection;
class QActionGroup;
class KActionMenu;
namespace MessageViewer {
class HeaderStyle;
class HeaderStrategy;
class HeaderStylePluginPrivate;
class HeaderStyleInterface;
class MESSAGEVIEWER_EXPORT HeaderStylePlugin : public QObject
{
    Q_OBJECT
public:
    explicit HeaderStylePlugin(QObject *parent = nullptr);
    ~HeaderStylePlugin();

    virtual HeaderStyle *headerStyle() const = 0;
    virtual HeaderStrategy *headerStrategy() const = 0;
    virtual HeaderStyleInterface *createView(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac, QObject *parent = nullptr) = 0;
    virtual QString name() const = 0;
    virtual bool hasMargin() const;
    virtual QString alignment() const;
    virtual int elidedTextSize() const;

    void setIsEnabled(bool enabled);
    bool isEnabled() const;

    virtual bool hasConfigureDialog() const;

    virtual void showConfigureDialog(QWidget *parent);

    virtual QString extraScreenCss(const QString &headerFont) const;
    virtual QString extraPrintCss(const QString &headerFont) const;
    virtual QString extraCommonCss(const QString &headerFont) const;

private:
    HeaderStylePluginPrivate *const d;
};
}
#endif // HEADERSTYLEPLUGIN_H
