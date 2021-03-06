/*
  Copyright (c) 2013-2019 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#include "scamdetectionwarningwidget.h"
#include "settings/messageviewersettings.h"

#include <KLocalizedString>
#include <QAction>
#include <QMenu>

using namespace MessageViewer;

class MessageViewer::ScamDetectionWarningWidgetPrivate
{
public:
    ScamDetectionWarningWidgetPrivate()
    {
    }

    bool mUseInTestApps = false;
};

ScamDetectionWarningWidget::ScamDetectionWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
    , d(new MessageViewer::ScamDetectionWarningWidgetPrivate)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("This message may be a scam. <a href=\"scamdetails\">(Details...)</a>"));

    connect(this, &ScamDetectionWarningWidget::linkActivated, this,
            &ScamDetectionWarningWidget::slotShowDetails);

    QMenu *menu = new QMenu(this);
    QAction *action = new QAction(i18n("Move to Trash"), this);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::moveMessageToTrash);
    action->setMenu(menu);
    addAction(action);

    action = new QAction(i18n("I confirm it's not a scam"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::slotMessageIsNotAScam);

    action = new QAction(i18n("Add email to whitelist"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::slotAddToWhiteList);

    action = new QAction(i18n("Disable scam detection for all messages"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this,
            &ScamDetectionWarningWidget::slotDisableScamDetection);
}

ScamDetectionWarningWidget::~ScamDetectionWarningWidget()
{
    delete d;
}

void ScamDetectionWarningWidget::setUseInTestApps(bool b)
{
    d->mUseInTestApps = b;
}

void ScamDetectionWarningWidget::slotMessageIsNotAScam()
{
    Q_EMIT messageIsNotAScam();
    setVisible(false);
}

void ScamDetectionWarningWidget::slotShowDetails(const QString &content)
{
    if (content == QLatin1String("scamdetails")) {
        Q_EMIT showDetails();
    }
}

void ScamDetectionWarningWidget::slotShowWarning()
{
    animatedShow();
}

void ScamDetectionWarningWidget::slotDisableScamDetection()
{
    if (!d->mUseInTestApps) {
        MessageViewer::MessageViewerSettings::self()->setScamDetectionEnabled(false);
        MessageViewer::MessageViewerSettings::self()->save();
    }
    setVisible(false);
}

void ScamDetectionWarningWidget::slotAddToWhiteList()
{
    setVisible(false);
    Q_EMIT addToWhiteList();
}
