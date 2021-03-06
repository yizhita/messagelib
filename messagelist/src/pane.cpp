/*
    Copyright (c) 2009 Kevin Ottens <ervin@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "pane.h"

#include <KActionCollection>
#include <KActionMenu>
#include <QIcon>
#include <KLocalizedString>
#include <QMenu>
#include <KXMLGUIClient>
#include <QAction>
#include <KToggleAction>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QItemSelectionModel>
#include <QTabBar>
#include <QToolButton>
#include <QMouseEvent>
#include <QHeaderView>
#include <QRegularExpression>

#include "storagemodel.h"
#include "core/widgets/quicksearchline.h"
#include "widget.h"
#include "messagelistsettings.h"
#include "core/manager.h"
#include <Akonadi/KMime/MessageStatus>
#include "core/model.h"
#include "messagelistutil_p.h"

namespace MessageList {
class Q_DECL_HIDDEN Pane::Private
{
public:
    Private(Pane *owner)
        : q(owner)
        , mXmlGuiClient(nullptr)
        , mActionMenu(nullptr)
        , mModel(nullptr)
        , mSelectionModel(nullptr)
        , mPreSelectionMode(Core::PreSelectLastSelected)
        , mNewTabButton(nullptr)
        , mCloseTabButton(nullptr)
        , mCloseTabAction(nullptr)
        , mActivateNextTabAction(nullptr)
        , mActivatePreviousTabAction(nullptr)
        , mMoveTabLeftAction(nullptr)
        , mMoveTabRightAction(nullptr)
        , mPreferEmptyTab(false)
        , mMaxTabCreated(0)
    {
    }

    void setCurrentFolder(const QModelIndex &etmIndex);
    void onNewTabClicked();
    void onCloseTabClicked();
    void activateTab();
    void closeTab(QWidget *);
    void onCurrentTabChanged();
    void onTabContextMenuRequest(const QPoint &pos);
    void activateNextTab();
    void activatePreviousTab();
    void moveTabLeft();
    void moveTabRight();
    void moveTabBackward();
    void moveTabForward();
    void changeQuicksearchVisibility(bool);
    void addActivateTabAction(int i);
    void slotTabCloseRequested(int index);
    QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;
    void updateTabControls();

    Pane *const q;

    KXMLGUIClient *mXmlGuiClient = nullptr;
    KActionMenu *mActionMenu = nullptr;

    QAbstractItemModel *mModel = nullptr;
    QItemSelectionModel *mSelectionModel = nullptr;
    Core::PreSelectionMode mPreSelectionMode;

    QHash<Widget *, QItemSelectionModel *> mWidgetSelectionHash;
    QVector<const QAbstractProxyModel *> mProxyStack;

    QToolButton *mNewTabButton = nullptr;
    QToolButton *mCloseTabButton = nullptr;
    QAction *mCloseTabAction = nullptr;
    QAction *mActivateNextTabAction = nullptr;
    QAction *mActivatePreviousTabAction = nullptr;
    QAction *mMoveTabLeftAction = nullptr;
    QAction *mMoveTabRightAction = nullptr;
    bool mPreferEmptyTab;
    int mMaxTabCreated;
};
} // namespace MessageList

using namespace Akonadi;
using namespace MessageList;

Pane::Pane(bool restoreSession, QAbstractItemModel *model, QItemSelectionModel *selectionModel, QWidget *parent)
    : QTabWidget(parent)
    , d(new Private(this))
{
    setDocumentMode(true);
    d->mModel = model;
    d->mSelectionModel = selectionModel;

    // Build the proxy stack
    const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel *>(d->mSelectionModel->model());

    while (proxyModel) {
        if (static_cast<const QAbstractItemModel *>(proxyModel) == d->mModel) {
            break;
        }

        d->mProxyStack << proxyModel;
        const QAbstractProxyModel *nextProxyModel = qobject_cast<const QAbstractProxyModel *>(proxyModel->sourceModel());

        if (!nextProxyModel) {
            // It's the final model in the chain, so it is necessarily the sourceModel.
            Q_ASSERT(qobject_cast<const QAbstractItemModel *>(proxyModel->sourceModel()) == d->mModel);
            break;
        }
        proxyModel = nextProxyModel;
    } // Proxy stack done

    d->mNewTabButton = new QToolButton(this);
    d->mNewTabButton->setIcon(QIcon::fromTheme(QStringLiteral("tab-new")));
    d->mNewTabButton->adjustSize();
    d->mNewTabButton->setToolTip(i18nc("@info:tooltip", "Open a new tab"));
#ifndef QT_NO_ACCESSIBILITY
    d->mNewTabButton->setAccessibleName(i18n("New tab"));
#endif
    setCornerWidget(d->mNewTabButton, Qt::TopLeftCorner);
    connect(d->mNewTabButton, &QToolButton::clicked, this, [this]() {
        d->onNewTabClicked();
    });

    d->mCloseTabButton = new QToolButton(this);
    d->mCloseTabButton->setIcon(QIcon::fromTheme(QStringLiteral("tab-close")));
    d->mCloseTabButton->adjustSize();
    d->mCloseTabButton->setToolTip(i18nc("@info:tooltip", "Close the current tab"));
#ifndef QT_NO_ACCESSIBILITY
    d->mCloseTabButton->setAccessibleName(i18n("Close tab"));
#endif
    setCornerWidget(d->mCloseTabButton, Qt::TopRightCorner);
    connect(d->mCloseTabButton, &QToolButton::clicked,
            this, [this]() {
        d->onCloseTabClicked();
    });

    setTabsClosable(true);
    connect(this, &Pane::tabCloseRequested, this, [this](int index) {
        d->slotTabCloseRequested(index);
    });

    readConfig(restoreSession);
    setMovable(true);

    connect(this, &Pane::currentChanged,
            this, [this]() {
        d->onCurrentTabChanged();
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &Pane::customContextMenuRequested, this, [this](const QPoint &point) {
        d->onTabContextMenuRequest(point);
    });

    connect(MessageListSettings::self(), &MessageListSettings::configChanged,
            this, [this]() {
        d->updateTabControls();
    });

    connect(this, &QTabWidget::tabBarDoubleClicked,
            this, &Pane::createNewTab);

    tabBar()->installEventFilter(this);
}

Pane::~Pane()
{
    saveCurrentSelection();
    writeConfig(true);
    delete d;
}

void Pane::Private::addActivateTabAction(int i)
{
    QString actionname;
    actionname.sprintf("activate_tab_%02d", i);
    QAction *action = new QAction(i18n("Activate Tab %1", i), q);
    mXmlGuiClient->actionCollection()->addAction(actionname, action);
    mXmlGuiClient->actionCollection()->setDefaultShortcut(action, QKeySequence(QStringLiteral("Alt+%1").arg(i)));
    connect(action, &QAction::triggered, q, [this]() {
        activateTab();
    });
}

void Pane::Private::slotTabCloseRequested(int index)
{
    QWidget *w = q->widget(index);
    if (w) {
        closeTab(w);
    }
}

void Pane::setXmlGuiClient(KXMLGUIClient *xmlGuiClient)
{
    d->mXmlGuiClient = xmlGuiClient;

    KToggleAction *const showHideQuicksearch = new KToggleAction(i18n("Show Quick Search Bar"), this);
    d->mXmlGuiClient->actionCollection()->setDefaultShortcut(showHideQuicksearch, Qt::CTRL + Qt::Key_H);
    showHideQuicksearch->setChecked(MessageListSettings::showQuickSearch());

    d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("show_quick_search"), showHideQuicksearch);
    connect(showHideQuicksearch, &KToggleAction::triggered, this, [this](bool state) {
        d->changeQuicksearchVisibility(state);
    });

    for (int i = 0; i < count(); ++i) {
        Widget *w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->setXmlGuiClient(d->mXmlGuiClient);
        }
    }

    // Setup "View->Message List" actions.
    if (xmlGuiClient) {
        if (d->mActionMenu) {
            d->mXmlGuiClient->actionCollection()->removeAction(d->mActionMenu);
        }
        d->mActionMenu = new KActionMenu(QIcon(), i18n("Message List"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("view_message_list"), d->mActionMenu);
        MessageList::Util::fillViewMenu(d->mActionMenu->menu(), this);

        d->mActionMenu->addSeparator();

        QAction *action = new QAction(i18n("Create New Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("create_new_tab"), action);
        d->mXmlGuiClient->actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
        connect(action, &QAction::triggered, this, [this]() {
            d->onNewTabClicked();
        });
        d->mActionMenu->addAction(action);

        d->mMaxTabCreated = count();
        for (int i = 1; i < 10 && i <= count(); ++i) {
            d->addActivateTabAction(i);
        }

        d->mCloseTabAction = new QAction(i18n("Close Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("close_current_tab"), d->mCloseTabAction);
        d->mXmlGuiClient->actionCollection()->setDefaultShortcuts(d->mCloseTabAction, QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_W) << QKeySequence(Qt::CTRL + Qt::Key_W));
        connect(d->mCloseTabAction, &QAction::triggered, this, [this]() {
            d->onCloseTabClicked();
        });
        d->mActionMenu->addAction(d->mCloseTabAction);
        d->mCloseTabAction->setEnabled(false);

        d->mActivateNextTabAction = new QAction(i18n("Activate Next Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("activate_next_tab"), d->mActivateNextTabAction);
        d->mActivateNextTabAction->setEnabled(false);
        connect(d->mActivateNextTabAction, &QAction::triggered, [this]() {
            d->activateNextTab();
        });

        d->mActivatePreviousTabAction = new QAction(i18n("Activate Previous Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("activate_previous_tab"), d->mActivatePreviousTabAction);
        d->mActivatePreviousTabAction->setEnabled(false);
        connect(d->mActivatePreviousTabAction, &QAction::triggered, this, [this]() {
            d->activatePreviousTab();
        });

        d->mMoveTabLeftAction = new QAction(i18n("Move Tab Left"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("move_tab_left"), d->mMoveTabLeftAction);
        d->mMoveTabLeftAction->setEnabled(false);
        connect(d->mMoveTabLeftAction, &QAction::triggered, this, [this]() {
            d->moveTabLeft();
        });

        d->mMoveTabRightAction = new QAction(i18n("Move Tab Right"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("move_tab_right"), d->mMoveTabRightAction);
        d->mMoveTabRightAction->setEnabled(false);
        connect(d->mMoveTabRightAction, &QAction::triggered, this, [this]() {
            d->moveTabRight();
        });
    }
}

bool Pane::selectNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectNextMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::selectPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectPreviousMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::focusNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->focusNextMessageItem(messageTypeFilter, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::focusPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->focusPreviousMessageItem(messageTypeFilter, centerItem, loop);
    } else {
        return false;
    }
}

void Pane::selectFocusedMessageItem(bool centerItem)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->selectFocusedMessageItem(centerItem);
    }
}

bool Pane::selectFirstMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectFirstMessageItem(messageTypeFilter, centerItem);
    } else {
        return false;
    }
}

bool Pane::selectLastMessageItem(Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectLastMessageItem(messageTypeFilter, centerItem);
    } else {
        return false;
    }
}

void Pane::selectAll()
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->selectAll();
    }
}

void Pane::setCurrentThreadExpanded(bool expand)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setCurrentThreadExpanded(expand);
    }
}

void Pane::setAllThreadsExpanded(bool expand)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setAllThreadsExpanded(expand);
    }
}

void Pane::setAllGroupsExpanded(bool expand)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setAllGroupsExpanded(expand);
    }
}

void Pane::focusQuickSearch(const QString &selectedText)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        w->focusQuickSearch(selectedText);
    }
}

void Pane::setQuickSearchClickMessage(const QString &msg)
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w) {
        w->setQuickSearchClickMessage(msg);
    }
}

void Pane::Private::setCurrentFolder(const QModelIndex &etmIndex)
{
    if (mPreferEmptyTab) {
        q->createNewTab();
    }

    Widget *w = static_cast<Widget *>(q->currentWidget());
    QItemSelectionModel *s = mWidgetSelectionHash[w];

    w->saveCurrentSelection();

    // Deselect old before we select new - so that the messagelist can clear first.
    s->clear();
    if (s->selection().isEmpty()) {
        w->view()->model()->setPreSelectionMode(mPreSelectionMode);
    }
    Q_ASSERT(s->model() == etmIndex.model());
    s->select(etmIndex, QItemSelectionModel::Select);

    QString label;
    QIcon icon;
    QString toolTip;
    for (const QModelIndex &index : s->selectedRows()) {
        label += index.data(Qt::DisplayRole).toString() + QLatin1String(", ");
    }
    label.chop(2);

    if (label.isEmpty()) {
        label = i18nc("@title:tab Empty messagelist", "Empty");
        icon = QIcon();
    } else if (s->selectedRows().size() == 1) {
        icon = s->selectedRows().first().data(Qt::DecorationRole).value<QIcon>();
        QModelIndex idx = s->selectedRows().first().parent();
        toolTip = label;
        while (idx != QModelIndex()) {
            toolTip = idx.data().toString() + QLatin1Char('/') + toolTip;
            idx = idx.parent();
        }
    } else {
        icon = QIcon::fromTheme(QStringLiteral("folder"));
    }

    const int index = q->indexOf(w);
    q->setTabText(index, label);
    q->setTabIcon(index, icon);
    q->setTabToolTip(index, toolTip);
    if (mPreferEmptyTab) {
        mSelectionModel->select(mapSelectionFromSource(s->selection()),
                                QItemSelectionModel::ClearAndSelect);
    }
    mPreferEmptyTab = false;
}

void Pane::Private::activateTab()
{
    q->tabBar()->setCurrentIndex(q->sender()->objectName().rightRef(2).toInt() - 1);
}

void Pane::Private::moveTabRight()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }
    if (QApplication::isRightToLeft()) {
        moveTabForward();
    } else {
        moveTabBackward();
    }
}

void Pane::Private::moveTabLeft()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }
    if (QApplication::isRightToLeft()) {
        moveTabBackward();
    } else {
        moveTabForward();
    }
}

void Pane::Private::moveTabForward()
{
    const int currentIndex = q->tabBar()->currentIndex();
    if (currentIndex == q->tabBar()->count() - 1) {
        return;
    }
    q->tabBar()->moveTab(currentIndex, currentIndex + 1);
}

void Pane::Private::moveTabBackward()
{
    const int currentIndex = q->tabBar()->currentIndex();
    if (currentIndex == 0) {
        return;
    }
    q->tabBar()->moveTab(currentIndex, currentIndex - 1);
}

void Pane::Private::activateNextTab()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }

    int indexTab = (q->tabBar()->currentIndex() + 1);

    if (indexTab == numberOfTab) {
        indexTab = 0;
    }

    q->tabBar()->setCurrentIndex(indexTab);
}

void Pane::Private::activatePreviousTab()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }

    int indexTab = (q->tabBar()->currentIndex() - 1);

    if (indexTab == -1) {
        indexTab = numberOfTab - 1;
    }

    q->tabBar()->setCurrentIndex(indexTab);
}

void Pane::Private::onNewTabClicked()
{
    q->createNewTab();
}

void Pane::Private::onCloseTabClicked()
{
    closeTab(q->currentWidget());
}

void Pane::Private::closeTab(QWidget *w)
{
    if (!w || (q->count() < 2)) {
        return;
    }

    Widget *wWidget = qobject_cast<Widget *>(w);
    if (wWidget) {
        wWidget->saveCurrentSelection();
    }

    delete w;
    updateTabControls();
}

void Pane::Private::changeQuicksearchVisibility(bool show)
{
    for (int i = 0; i < q->count(); ++i) {
        Widget *w = qobject_cast<Widget *>(q->widget(i));
        if (w) {
            w->changeQuicksearchVisibility(show);
        }
    }
}

bool Pane::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *const mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::MidButton) {
            return true;
        }
    }
    return QTabWidget::eventFilter(object, event);
}

void Pane::Private::onCurrentTabChanged()
{
    Q_EMIT q->currentTabChanged();

    Widget *w = static_cast<Widget *>(q->currentWidget());

    QItemSelectionModel *s = mWidgetSelectionHash[w];

    mSelectionModel->select(mapSelectionFromSource(s->selection()),
                            QItemSelectionModel::ClearAndSelect);
}

void Pane::Private::onTabContextMenuRequest(const QPoint &pos)
{
    QTabBar *bar = q->tabBar();
    if (q->count() <= 1) {
        return;
    }

    const int indexBar = bar->tabAt(bar->mapFrom(q, pos));
    if (indexBar == -1) {
        return;
    }

    Widget *w = qobject_cast<Widget *>(q->widget(indexBar));
    if (!w) {
        return;
    }

    QMenu menu(q);

    QAction *closeTabAction = menu.addAction(i18nc("@action:inmenu", "Close Tab"));
    closeTabAction->setIcon(QIcon::fromTheme(QStringLiteral("tab-close")));

    QAction *allOther = menu.addAction(i18nc("@action:inmenu", "Close All Other Tabs"));
    allOther->setIcon(QIcon::fromTheme(QStringLiteral("tab-close-other")));

    QAction *action = menu.exec(q->mapToGlobal(pos));

    if (action == allOther) {   // Close all other tabs
        QList<Widget *> widgets;
        const int index = q->indexOf(w);

        for (int i = 0; i < q->count(); ++i) {
            if (i == index) {
                continue;    // Skip the current one
            }

            Widget *other = qobject_cast<Widget *>(q->widget(i));
            if (other) {
                widgets << other;
            }
        }

        foreach (Widget *other, widgets) {
            other->saveCurrentSelection();
            delete other;
        }

        updateTabControls();
    } else if (action == closeTabAction) {
        closeTab(q->widget(indexBar));
    }
}

MessageList::StorageModel *Pane::createStorageModel(QAbstractItemModel *model, QItemSelectionModel *selectionModel, QObject *parent)
{
    return new MessageList::StorageModel(model, selectionModel, parent);
}

Akonadi::Collection Pane::currentFolder() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentCollection();
    }
    return {};
}

void Pane::setCurrentFolder(const Akonadi::Collection &collection, const QModelIndex &etmIndex, bool, Core::PreSelectionMode preSelectionMode, const QString &overrideLabel)
{
    d->setCurrentFolder(etmIndex);
    d->mPreSelectionMode = preSelectionMode;
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        w->setCurrentFolder(collection);
        QItemSelectionModel *s = d->mWidgetSelectionHash[w];
        MessageList::StorageModel *m = createStorageModel(d->mModel, s, w);
        w->setStorageModel(m, preSelectionMode);
        if (!overrideLabel.isEmpty()) {
            int index = indexOf(w);
            setTabText(index, overrideLabel);
        }
    }
}

void Pane::updateTabIconText(const Akonadi::Collection &collection, const QString &label, const QIcon &icon)
{
    for (int i = 0; i < count(); ++i) {
        Widget *w = qobject_cast<Widget *>(widget(i));
        if (w && (w->currentCollection() == collection)) {
            const int index = indexOf(w);
            setTabText(index, label);
            setTabIcon(index, icon);
        }
    }
}

QItemSelectionModel *Pane::createNewTab()
{
    Widget *w = new Widget(this);
    w->setXmlGuiClient(d->mXmlGuiClient);

    addTab(w, i18nc("@title:tab Empty messagelist", "Empty"));

    if (d->mXmlGuiClient && count() < 10) {
        if (d->mMaxTabCreated < count()) {
            d->mMaxTabCreated = count();
            d->addActivateTabAction(d->mMaxTabCreated);
        }
    }

    QItemSelectionModel *s = new QItemSelectionModel(d->mModel, w);
    MessageList::StorageModel *m = createStorageModel(d->mModel, s, w);
    w->setStorageModel(m);

    d->mWidgetSelectionHash[w] = s;

    connect(w, &Widget::messageSelected,
            this, &Pane::messageSelected);
    connect(w, &Widget::messageActivated,
            this, &Pane::messageActivated);
    connect(w, &Widget::selectionChanged,
            this, &Pane::selectionChanged);
    connect(w, &Widget::messageStatusChangeRequest,
            this, &Pane::messageStatusChangeRequest);

    connect(w, &Core::Widget::statusMessage,
            this, &Pane::statusMessage);

    connect(w, &Core::Widget::forceLostFocus,
            this, &Pane::forceLostFocus);

    d->updateTabControls();
    setCurrentWidget(w);
    return s;
}

QItemSelection Pane::Private::mapSelectionFromSource(const QItemSelection &selection) const
{
    QItemSelection result = selection;

    typedef QVector<const QAbstractProxyModel *>::ConstIterator Iterator;

    for (Iterator it = mProxyStack.end() - 1; it != mProxyStack.begin(); --it) {
        result = (*it)->mapSelectionFromSource(result);
    }
    result = mProxyStack.first()->mapSelectionFromSource(result);

    return result;
}

void Pane::Private::updateTabControls()
{
    const bool enableAction = (q->count() > 1);
    if (enableAction) {
        q->setCornerWidget(mCloseTabButton, Qt::TopRightCorner);
        mCloseTabButton->setVisible(true);
    } else {
        q->setCornerWidget(nullptr, Qt::TopRightCorner);
    }
    if (mCloseTabAction) {
        mCloseTabAction->setEnabled(enableAction);
    }
    if (mActivatePreviousTabAction) {
        mActivatePreviousTabAction->setEnabled(enableAction);
    }
    if (mActivateNextTabAction) {
        mActivateNextTabAction->setEnabled(enableAction);
    }
    if (mMoveTabRightAction) {
        mMoveTabRightAction->setEnabled(enableAction);
    }
    if (mMoveTabLeftAction) {
        mMoveTabLeftAction->setEnabled(enableAction);
    }

    q->tabBar()->setVisible(enableAction);
    if (enableAction) {
        q->setCornerWidget(mNewTabButton, Qt::TopLeftCorner);
        mNewTabButton->setVisible(true);
    } else {
        q->setCornerWidget(nullptr, Qt::TopLeftCorner);
    }

    q->setTabsClosable(true);
    const int numberOfTab(q->count());
    if (numberOfTab == 1) {
        q->tabBar()->tabButton(0, QTabBar::RightSide)->setEnabled(false);
    } else if (numberOfTab > 1) {
        q->tabBar()->tabButton(0, QTabBar::RightSide)->setEnabled(true);
    }
}

Item Pane::currentItem() const
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w == nullptr) {
        return Item();
    }

    return w->currentItem();
}

KMime::Message::Ptr Pane::currentMessage() const
{
    Widget *w = static_cast<Widget *>(currentWidget());

    if (w == nullptr) {
        return KMime::Message::Ptr();
    }

    return w->currentMessage();
}

QList<KMime::Message::Ptr > Pane::selectionAsMessageList(bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return QList<KMime::Message::Ptr>();
    }
    return w->selectionAsMessageList(includeCollapsedChildren);
}

Akonadi::Item::List Pane::selectionAsMessageItemList(bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return Akonadi::Item::List();
    }
    return w->selectionAsMessageItemList(includeCollapsedChildren);
}

QList<Akonadi::Item::Id> Pane::selectionAsListMessageId(bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return QList<Akonadi::Item::Id>();
    }
    return w->selectionAsListMessageId(includeCollapsedChildren);
}

QVector<qlonglong> Pane::selectionAsMessageItemListId(bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return QVector<qlonglong>();
    }
    return w->selectionAsMessageItemListId(includeCollapsedChildren);
}

Akonadi::Item::List Pane::currentThreadAsMessageList() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return Akonadi::Item::List();
    }
    return w->currentThreadAsMessageList();
}

Akonadi::Item::List Pane::itemListFromPersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->itemListFromPersistentSet(ref);
    }
    return Akonadi::Item::List();
}

void Pane::deletePersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        w->deletePersistentSet(ref);
    }
}

void Pane::markMessageItemsAsAboutToBeRemoved(MessageList::Core::MessageItemSetReference ref, bool bMark)
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        w->markMessageItemsAsAboutToBeRemoved(ref, bMark);
    }
}

QList<Akonadi::MessageStatus> Pane::currentFilterStatus() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return QList<Akonadi::MessageStatus>();
    }
    return w->currentFilterStatus();
}

Core::QuickSearchLine::SearchOptions Pane::currentOptions() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return Core::QuickSearchLine::SearchEveryWhere;
    }
    return w->currentOptions();
}

QString Pane::currentFilterSearchString() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentFilterSearchString();
    }
    return QString();
}

bool Pane::isThreaded() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->isThreaded();
    }
    return false;
}

bool Pane::selectionEmpty() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->selectionEmpty();
    }
    return false;
}

bool Pane::getSelectionStats(Akonadi::Item::List &selectedItems, Akonadi::Item::List &selectedVisibleItems, bool *allSelectedBelongToSameThread, bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w == nullptr) {
        return false;
    }

    return w->getSelectionStats(
        selectedItems, selectedVisibleItems,
        allSelectedBelongToSameThread, includeCollapsedChildren
        );
}

MessageList::Core::MessageItemSetReference Pane::selectionAsPersistentSet(bool includeCollapsedChildren) const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->selectionAsPersistentSet(includeCollapsedChildren);
    }
    return -1;
}

MessageList::Core::MessageItemSetReference Pane::currentThreadAsPersistentSet() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentThreadAsPersistentSet();
    }
    return -1;
}

void Pane::focusView()
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        QWidget *view = w->view();
        if (view) {
            view->setFocus();
        }
    }
}

void Pane::reloadGlobalConfiguration()
{
    d->updateTabControls();
}

QItemSelectionModel *Pane::currentItemSelectionModel()
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->view()->selectionModel();
    }
    return nullptr;
}

void Pane::resetModelStorage()
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        MessageList::StorageModel *m = static_cast<MessageList::StorageModel *>(w->storageModel());
        if (m) {
            m->resetModelStorage();
        }
    }
}

void Pane::setPreferEmptyTab(bool emptyTab)
{
    d->mPreferEmptyTab = emptyTab;
}

void Pane::saveCurrentSelection()
{
    for (int i = 0; i < count(); ++i) {
        Widget *w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->saveCurrentSelection();
        }
    }
}

void Pane::updateTagComboBox()
{
    for (int i = 0; i < count(); ++i) {
        Widget *w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->populateStatusFilterCombo();
        }
    }
}

void Pane::writeConfig(bool restoreSession)
{
    KConfigGroup conf(MessageList::MessageListSettings::self()->config(), "MessageListPane");

    // Delete list before
    const QStringList list = MessageList::MessageListSettings::self()->config()->groupList().filter(QRegularExpression(QStringLiteral("MessageListTab\\d+")));
    for (const QString &group : list) {
        MessageList::MessageListSettings::self()->config()->deleteGroup(group);
    }

    if (restoreSession) {
        conf.writeEntry(QStringLiteral("currentIndex"), currentIndex());
        conf.writeEntry(QStringLiteral("tabNumber"), count());

        for (int i = 0; i < count(); ++i) {
            Widget *w = qobject_cast<Widget *>(widget(i));
            if (w) {
                KConfigGroup grp(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListTab%1").arg(i));
                grp.writeEntry(QStringLiteral("collectionId"), w->currentCollection().id());
                grp.writeEntry(QStringLiteral("HeaderState"), w->view()->header()->saveState());
            }
        }
    }
    conf.sync();
}

void Pane::readConfig(bool restoreSession)
{
    if (MessageList::MessageListSettings::self()->config()->hasGroup(QStringLiteral("MessageListPane"))) {
        KConfigGroup conf(MessageList::MessageListSettings::self()->config(), "MessageListPane");
        const int numberOfTab = conf.readEntry(QStringLiteral("tabNumber"), 0);
        if (numberOfTab == 0) {
            createNewTab();
        } else {
            for (int i = 0; i < numberOfTab; ++i) {
                createNewTab();
                restoreHeaderSettings(i);
                if (restoreSession) {
#if 0 //TODO fix me
                    Akonadi::Collection::Id id = grp.readEntry(QStringLiteral("collectionId"), -1);
                    ETMViewStateSaver *saver = new ETMViewStateSaver;
                    saver->setSelectionModel(selectionModel);

                    if (id != -1) {
                        ETMViewStateSaver *saver = new ETMViewStateSaver;
                        saver->setSelectionModel(selectionModel);
                        saver->restoreState(grp);
                        saver->selectCollections(Akonadi::Collection::List() << Akonadi::Collection(id));
                        saver->restoreCurrentItem(QString::fromLatin1("c%1").arg(id));
                    }
#endif
                }
            }
            setCurrentIndex(conf.readEntry(QStringLiteral("currentIndex"), 0));
        }
    } else {
        createNewTab();
        restoreHeaderSettings(0);
    }
}

void Pane::restoreHeaderSettings(int index)
{
    KConfigGroup grp(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListTab%1").arg(index));
    if (grp.exists()) {
        Widget *w = qobject_cast<Widget *>(widget(index));
        if (w) {
            w->view()->header()->restoreState(grp.readEntry(QStringLiteral("HeaderState"), QByteArray()));
        }
    }
}

bool Pane::searchEditHasFocus() const
{
    Widget *w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->searchEditHasFocus();
    }
    return false;
}

void Pane::sortOrderMenuAboutToShow()
{
    QMenu *menu = qobject_cast< QMenu * >(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->sortOrderMenuAboutToShow(menu);
}

void Pane::aggregationMenuAboutToShow()
{
    QMenu *menu = qobject_cast< QMenu * >(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->aggregationMenuAboutToShow(menu);
}

void Pane::themeMenuAboutToShow()
{
    QMenu *menu = qobject_cast< QMenu * >(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->themeMenuAboutToShow(menu);
}

void Pane::populateStatusFilterCombo()
{
    for (int i = 0; i < count(); ++i) {
        Widget *w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->populateStatusFilterCombo();
        }
    }
}

#include "moc_pane.cpp"
