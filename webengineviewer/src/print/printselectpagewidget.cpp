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

#include "printselectpagewidget.h"
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QListWidget>

using namespace WebEngineViewer;
PrintSelectPageWidget::PrintSelectPageWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setMargin(0);

    mListPage = new QListWidget(this);
    mListPage->setObjectName(QStringLiteral("listpage"));
    mainLayout->addWidget(mListPage);
}

PrintSelectPageWidget::~PrintSelectPageWidget()
{

}

void PrintSelectPageWidget::setPages(int page)
{
    for (int i = 0; i < page; ++i) {
        QListWidgetItem *item = new QListWidgetItem(i18n("Page %1", (i + 1)), mListPage);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(PageIndex, i);
    }
}

QList<int> PrintSelectPageWidget::pages() const
{
    QList<int> listPages;
    for (int i = 0; i < mListPage->count(); ++i) {
        QListWidgetItem *item = mListPage->item(i);
        if (item->checkState() == Qt::Checked) {
            listPages.append(item->data(PageIndex).toInt());
        }
    }
    return listPages;
}