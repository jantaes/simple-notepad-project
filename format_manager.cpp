//
// Created by Shady  on 16/5/26.
//

#include "format_manager.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QTextCursor>

FormatManager::FormatManager(QObject *parent)
    : QObject(parent) {
}

void FormatManager::setEditor(QTextEdit *editor) {
    if (this->editor) {
        disconnect(this->editor, &QTextEdit::currentCharFormatChanged,
                   this, &FormatManager::updateFormatActions);
    }

    this->editor = editor;

    if (this->editor) {
        connect(this->editor, &QTextEdit::currentCharFormatChanged,
                this, &FormatManager::updateFormatActions);
    }
}

void FormatManager::setupConnections() {
    connect(action_bold, &QAction::triggered, this, &FormatManager::toggleBold);
    connect(action_italic, &QAction::triggered, this, &FormatManager::toggleItalic);
    connect(action_underline, &QAction::triggered, this, &FormatManager::toggleUnderline);
    connect(action_strikethrough, &QAction::triggered, this, &FormatManager::toggleStrikethrough);

    connect(action_font, &QAction::triggered, this, &FormatManager::showFontDialog);
    connect(action_text_color, &QAction::triggered, this, &FormatManager::showColorDialog);
}

void FormatManager::setupFormatMenu(QMenu *formatMenu) {
    action_font = formatMenu->addAction("Font...");
    connect(action_font, &QAction::triggered, this, &FormatManager::showFontDialog);

    action_text_color = formatMenu->addAction("Text Color...");
    connect(action_text_color, &QAction::triggered, this, &FormatManager::showColorDialog);
}

void FormatManager::setupFormatToolbar(QToolBar *toolbar, QTextEdit *editor) {
    format_toolbar = toolbar;
    setEditor(editor);

    toolbar->setIconSize(QSize(16, 16));

    action_bold = toolbar->addAction(QIcon("data/images/bold.svg"), "Bold");
    action_bold->setCheckable(true);
    action_bold->setShortcut(QKeySequence::Bold);

    action_italic = toolbar->addAction(QIcon("data/images/italic.svg"), "Italic");
    action_italic->setCheckable(true);
    action_italic->setShortcut(QKeySequence::Italic);

    action_underline = toolbar->addAction(QIcon("data/images/underline.svg"), "Underline");
    action_underline->setCheckable(true);
    action_underline->setShortcut(QKeySequence::Underline);

    action_strikethrough = toolbar->addAction(QIcon("data/images/strikethrough.svg"), "Strikethrough");
    action_strikethrough->setCheckable(true);
    action_strikethrough->setShortcut(QKeySequence("Ctrl+D")); // нету встроенной горячей клавишы, ctrl+d для удобства

    setupConnections();
}

void FormatManager::toggleBold(bool checked) {
    if (!editor) return;

    QTextCharFormat fmt;
    fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    editor->mergeCurrentCharFormat(fmt);
}

void FormatManager::toggleItalic(bool checked) {
    if (!editor) return;

    QTextCharFormat fmt;
    fmt.setFontItalic(checked);
    editor->mergeCurrentCharFormat(fmt);
}

void FormatManager::toggleUnderline(bool checked) {
    if (!editor) return;

    QTextCharFormat fmt;
    fmt.setFontUnderline(checked);
    editor->mergeCurrentCharFormat(fmt);
}

void FormatManager::toggleStrikethrough(bool checked) {
    if (!editor) return;

    QTextCharFormat fmt;
    fmt.setFontStrikeOut(checked);
    editor->mergeCurrentCharFormat(fmt);
}

void FormatManager::showFontDialog() {
    if (!editor) return;

    bool ok;
    QFont font;

    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        QTextCharFormat format = cursor.charFormat();
        font = format.font();
    } else {
        font = editor->currentFont();
    }

    font = QFontDialog::getFont(&ok, font, editor);

    if (ok) {
        QTextCharFormat fmt;
        fmt.setFont(font);

        QTextCursor editCursor = editor->textCursor();
        if (!editCursor.hasSelection()) {
            editCursor.select(QTextCursor::Document);
        }

        editCursor.mergeCharFormat(fmt);
        editor->setTextCursor(editCursor);
    }
}

void FormatManager::showColorDialog() {
    if (!editor) return;

    QColor color;

    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        QTextCharFormat format = cursor.charFormat();
        color = format.foreground().color();
    } else {
        color = editor->textColor();
    }

    color = QColorDialog::getColor(color, editor, "Select Text Color");

    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);

        QTextCursor editCursor = editor->textCursor();
        if (!editCursor.hasSelection()) {
            editCursor.select(QTextCursor::Document);
        }

        editCursor.mergeCharFormat(fmt);
        editor->setTextCursor(editCursor);
    }
}

void FormatManager::updateFormatActions(const QTextCharFormat &format) {
    if (action_bold) {
        action_bold->setChecked(format.fontWeight() == QFont::Bold);
    }
    if (action_italic) {
        action_italic->setChecked(format.fontItalic());
    }
    if (action_underline) {
        action_underline->setChecked(format.fontUnderline());
    }
    if (action_strikethrough) {
        action_strikethrough->setChecked(format.fontStrikeOut());
    }
}
