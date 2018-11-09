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

#ifndef DKIMCONFIGUREDIALOG_H
#define DKIMCONFIGUREDIALOG_H

#include <QDialog>
#include "messageviewer_private_export.h"

namespace MessageViewer {
class DKIMConfigureWidget;
class MESSAGEVIEWER_TESTS_EXPORT DKIMConfigureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DKIMConfigureDialog(QWidget *parent = nullptr);
    ~DKIMConfigureDialog();

private:
    void slotReset();
    void readConfig();
    void slotAccepted();
    void writeConfig();
    DKIMConfigureWidget *mConfigureWidget = nullptr;
};
}
#endif // DKIMCONFIGUREDIALOG_H
