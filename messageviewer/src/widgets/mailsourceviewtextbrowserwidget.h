/*
 *
 *  This file is part of KMail, the KDE mail client.
 *
 *  Copyright (c) 2002-2003 Carsten Pfeiffer <pfeiffer@kde.org>
 *  Copyright (c) 2003      Zack Rusin <zack@kde.org>
 *
 *  KMail is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  KMail is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of this program with any edition of
 *  the Qt library by Trolltech AS, Norway (or with modified versions
 *  of Qt that use the same license as Qt), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt.  If you modify this file, you may extend this exception to
 *  your version of the file, but you are not obligated to do so.  If
 *  you do not wish to do so, delete this exception statement from
 *  your version.
 */
#ifndef MAILSOURCEVIEWTEXTBROWSERWIDGET_H
#define MAILSOURCEVIEWTEXTBROWSERWIDGET_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <KSyntaxHighlighting/Repository>
namespace KPIMTextEdit {
class SlideContainer;
class TextToSpeechWidget;
class TextToSpeechInterface;
}

namespace MessageViewer {
class FindBarSourceView;

/**
 * A tiny little class to use for displaying raw messages, textual
 * attachments etc.
 *
 * Auto-deletes itself when closed.
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class MailSourceViewTextBrowser;

class MailSourceViewTextBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MailSourceViewTextBrowserWidget(const QString &syntax, QWidget *parent = nullptr);

    void setText(const QString &text);
    void setPlainText(const QString &text);
    void setFixedFont();
    Q_REQUIRED_RESULT MessageViewer::MailSourceViewTextBrowser *textBrowser() const;
private:
    void slotFind();
    KSyntaxHighlighting::Repository mRepo;
    MailSourceViewTextBrowser *mTextBrowser = nullptr;
    FindBarSourceView *mFindBar = nullptr;
    KPIMTextEdit::SlideContainer *mSliderContainer = nullptr;
    KPIMTextEdit::TextToSpeechWidget *mTextToSpeechWidget = nullptr;
};

class MailSourceViewTextBrowser : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MailSourceViewTextBrowser(KPIMTextEdit::TextToSpeechInterface *textToSpeechInterface, QWidget *parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
Q_SIGNALS:
    void findText();
private:
    void slotSpeakText();
    void slotSaveAs();
    KPIMTextEdit::TextToSpeechInterface *mTextToSpeechInterface = nullptr;
};
}
#endif // MAILSOURCEVIEWTEXTBROWSERWIDGET_H
