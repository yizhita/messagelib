/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#ifndef TEMPLATESINSERTCOMMANDACTION_H
#define TEMPLATESINSERTCOMMANDACTION_H

#include <QWidgetAction>
#include <TemplateParser/TemplatesCommandMenu>

#include "templateparser_export.h"
namespace TemplateParser {
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandAction(QObject *parent = nullptr);
    ~TemplatesInsertCommandAction();

    TemplatesCommandMenu::MenuTypes type() const;
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    void insertCommand(const QString &cmd, int adjustCursor);

private:
    TemplatesCommandMenu *mMenuCommand = nullptr;
};
}
#endif // TEMPLATESINSERTCOMMANDACTION_H
