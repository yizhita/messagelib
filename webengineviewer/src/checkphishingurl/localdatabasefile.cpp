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

#include "localdatabasefile.h"
#include <QFileInfo>
#include <QtEndian>


using namespace WebEngineViewer;

class WebEngineViewer::LocalDataBaseFilePrivate
{
public:
    LocalDataBaseFilePrivate(const QString &filename, LocalDataBaseFile *qq)
        : mFile(filename),
          mData(Q_NULLPTR),
          q(qq),
          mValid(false)
    {
        load();
    }
    bool load();
    bool reload();

    QFile mFile;
    uchar *mData;
    QDateTime mMtime;
    LocalDataBaseFile *q;
    bool mValid;
};

bool LocalDataBaseFilePrivate::load()
{
    if (!mFile.open(QIODevice::ReadOnly))
        return false;
    mData = mFile.map(0, mFile.size());
    if (mData) {
        const int major = q->getUint16(0);
        const int minor = q->getUint16(2);
        mValid = (major == 1 && minor >= 1 && minor <= 2);
    }
    mMtime = QFileInfo(mFile).lastModified();
    return mValid;
}

bool LocalDataBaseFilePrivate::reload()
{
    mValid = false;
    if (mFile.isOpen()) {
        mFile.close();
    }
    mData = 0;
    return load();
}


LocalDataBaseFile::LocalDataBaseFile(const QString &filename)
    : d(new WebEngineViewer::LocalDataBaseFilePrivate(filename, this))
{
}

LocalDataBaseFile::~LocalDataBaseFile()
{
    delete d;
}

bool LocalDataBaseFile::isValid() const
{
    return d->mValid;
}

quint16 LocalDataBaseFile::getUint16(int offset) const
{
    return qFromBigEndian(*reinterpret_cast<quint16 *>(d->mData + offset));
}

quint32 LocalDataBaseFile::getUint32(int offset) const
{
    return qFromBigEndian(*reinterpret_cast<quint32 *>(d->mData + offset));
}

const char *LocalDataBaseFile::getCharStar(int offset) const
{
    return reinterpret_cast<const char *>(d->mData + offset);
}

bool LocalDataBaseFile::reload()
{
    return d->reload();
}