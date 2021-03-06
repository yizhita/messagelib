/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#include "templateparseremailaddressrequesterlineedit.h"
#include <QHBoxLayout>
#include <QLineEdit>

using namespace TemplateParser;
TemplateParserEmailAddressRequesterLineEdit::TemplateParserEmailAddressRequesterLineEdit(QWidget *parent)
    : TemplateParser::TemplateParserEmailAddressRequesterBase(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mLineEdit = new QLineEdit(this);
    mLineEdit->setObjectName(QStringLiteral("lineedit"));
    mainLayout->addWidget(mLineEdit);
    connect(mLineEdit, &QLineEdit::textChanged, this, &TemplateParserEmailAddressRequesterLineEdit::textChanged);
}

TemplateParserEmailAddressRequesterLineEdit::~TemplateParserEmailAddressRequesterLineEdit()
{
    disconnect(mLineEdit, &QLineEdit::textChanged, this, &TemplateParserEmailAddressRequesterLineEdit::textChanged);
}

QString TemplateParserEmailAddressRequesterLineEdit::text() const
{
    return mLineEdit->text();
}

void TemplateParserEmailAddressRequesterLineEdit::setText(const QString &str)
{
    mLineEdit->setText(str);
}

void TemplateParserEmailAddressRequesterLineEdit::clear()
{
    mLineEdit->clear();
}
