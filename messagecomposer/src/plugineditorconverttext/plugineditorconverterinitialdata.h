/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORCONVERTERINITIALDATA_H
#define PLUGINEDITORCONVERTERINITIALDATA_H

#include "messagecomposer_export.h"
#include <KMime/Message>
namespace MessageComposer {
class MESSAGECOMPOSER_EXPORT PluginEditorConverterInitialData
{
public:
    PluginEditorConverterInitialData();

    Q_REQUIRED_RESULT bool newMessage() const;
    void setNewMessage(bool newMessage);

    Q_REQUIRED_RESULT KMime::Message::Ptr mewMsg() const;
    void setMewMsg(const KMime::Message::Ptr &mewMsg);

private:
    KMime::Message::Ptr mMewMsg;
    bool mNewMessage = true;
};
}
#endif // PLUGINEDITORCONVERTERINITIALDATA_H
