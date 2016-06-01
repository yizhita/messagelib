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

#ifndef PLUGINEDITORCHECKBEFORESENDINTERFACE_H
#define PLUGINEDITORCHECKBEFORESENDINTERFACE_H

#include <QObject>
#include "messagecomposer_export.h"
namespace MessageComposer
{
class PluginEditorCheckBeforeSendInterfacePrivate;
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendInterface(QObject *parent = Q_NULLPTR);
    ~PluginEditorCheckBeforeSendInterface();

    virtual bool hasConfigureDialog() const;

    virtual QWidget *createConfigureWidget(QWidget *parent) const;

    virtual void exec() = 0;

    void setParentWidget(QWidget *parent);
    QWidget *parentWidget() const;


private:
    PluginEditorCheckBeforeSendInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORCHECKBEFORESENDINTERFACE_H
