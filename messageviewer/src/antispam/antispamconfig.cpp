/*
    antispamconfig.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2004 Patrick Audley <paudley@blackcat.ca>
    Copyright (c) 2004 Ingo Kloecker <kloecker@kde.org>

    KMail is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "antispamconfig.h"

#include <kconfig.h>
#include <kconfiggroup.h>

using namespace MessageViewer;

AntiSpamConfig *AntiSpamConfig::instance()
{
    static AntiSpamConfig s_self;
    return &s_self;
}

const SpamAgents AntiSpamConfig::agents() const
{
    return mAgents;
}

AntiSpamConfig::AntiSpamConfig()
{
    readConfig();
}

AntiSpamConfig::~AntiSpamConfig()
{
}

void AntiSpamConfig::readConfig()
{
    mAgents.clear();
    KConfig config(QStringLiteral("kmail.antispamrc"));
    config.setReadDefaults(true);
    KConfigGroup general(&config, "General");
    int totalTools = general.readEntry("tools", 0);
    for (int i = 1; i <= totalTools; ++i) {
        KConfigGroup tool(&config, QStringLiteral("Spamtool #%1").arg(i));
        if (tool.hasKey("ScoreHeader")) {
            const QString name = tool.readEntry("ScoreName");
            const QByteArray header = tool.readEntry("ScoreHeader").toLatin1();
            const QByteArray cheader = tool.readEntry("ConfidenceHeader").toLatin1();
            const QByteArray type = tool.readEntry("ScoreType").toLatin1();
            const QString score = tool.readEntryUntranslated("ScoreValueRegexp");
            const QString threshold = tool.readEntryUntranslated("ScoreThresholdRegexp");
            const QString confidence = tool.readEntryUntranslated("ScoreConfidenceRegexp");
            SpamAgentTypes typeE = SpamAgentNone;
            if (qstricmp(type.data(), "bool") == 0) {
                typeE = SpamAgentBool;
            } else if (qstricmp(type.data(), "decimal") == 0) {
                typeE = SpamAgentFloat;
            } else if (qstricmp(type.data(), "percentage") == 0) {
                typeE = SpamAgentFloatLarge;
            } else if (qstricmp(type.data(), "adjusted") == 0) {
                typeE = SpamAgentAdjustedFloat;
            }
            mAgents.append(SpamAgent(name, typeE, header, cheader, QRegExp(score),
                                     QRegExp(threshold), QRegExp(confidence)));
        }
    }
}

const SpamAgents AntiSpamConfig::uniqueAgents() const
{
    QStringList seenAgents;
    SpamAgents agents;
    SpamAgents::ConstIterator it(mAgents.begin());
    SpamAgents::ConstIterator end(mAgents.end());
    for (; it != end; ++it) {
        const QString agent((*it).name());
        if (!seenAgents.contains(agent)) {
            agents.append(*it);
            seenAgents.append(agent);
        }
    }
    return agents;
}
