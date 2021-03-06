/*
   Copyright (C) 2013-2019 Laurent Montel <montel@kde.org>

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

#include "grantleeheaderteststyle.h"

#include "header/grantleeheaderformatter.h"
#include "header/headerstrategy.h"

#include <kmime/kmime_message.h>

using namespace MessageViewer;

class MessageViewer::GrantleeHeaderTestStylePrivate
{
public:
    GrantleeHeaderTestStylePrivate()
        : mGrantleeFormatter(new GrantleeHeaderFormatter)
    {
    }

    ~GrantleeHeaderTestStylePrivate()
    {
        delete mGrantleeFormatter;
    }

    QStringList mExtraDisplay;
    QString mAbsolutePath;
    QString mMainFilename;
    GrantleeHeaderFormatter *mGrantleeFormatter = nullptr;
};

GrantleeHeaderTestStyle::GrantleeHeaderTestStyle()
    : HeaderStyle()
    , d(new MessageViewer::GrantleeHeaderTestStylePrivate)
{
}

GrantleeHeaderTestStyle::~GrantleeHeaderTestStyle()
{
    delete d;
}

const char *GrantleeHeaderTestStyle::name() const
{
    return "grantleetest";
}

QString GrantleeHeaderTestStyle::format(KMime::Message *message) const
{
    if (!message) {
        return QString();
    }
    return d->mGrantleeFormatter->toHtml(d->mExtraDisplay, d->mAbsolutePath, d->mMainFilename, this,
                                         message, isPrinting());
}

void GrantleeHeaderTestStyle::setAbsolutePath(const QString &path)
{
    d->mAbsolutePath = path;
}

void GrantleeHeaderTestStyle::setMainFilename(const QString &filename)
{
    d->mMainFilename = filename;
}

void GrantleeHeaderTestStyle::setExtraDisplayHeaders(const QStringList &extraDisplay)
{
    d->mExtraDisplay = extraDisplay;
}
