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

#include "plugineditorcheckbeforesendinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorCheckBeforeSendInterfacePrivate
{
public:
    PluginEditorCheckBeforeSendInterfacePrivate()
    {
    }

    MessageComposer::PluginEditorCheckBeforeSendParams parameters;
    QWidget *mParentWidget = nullptr;
};

PluginEditorCheckBeforeSendInterface::PluginEditorCheckBeforeSendInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorCheckBeforeSendInterfacePrivate)
{
}

PluginEditorCheckBeforeSendInterface::~PluginEditorCheckBeforeSendInterface()
{
    delete d;
}

void PluginEditorCheckBeforeSendInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorCheckBeforeSendInterface::parentWidget() const
{
    return d->mParentWidget;
}

void PluginEditorCheckBeforeSendInterface::setParameters(const MessageComposer::PluginEditorCheckBeforeSendParams &params)
{
    d->parameters = params;
}

MessageComposer::PluginEditorCheckBeforeSendParams PluginEditorCheckBeforeSendInterface::parameters() const
{
    return d->parameters;
}

void PluginEditorCheckBeforeSendInterface::reloadConfig()
{
    //Reimplement it
}
