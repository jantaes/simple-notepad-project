//
// Created by Shady  on 16/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_FORMATMANAGER_H
#define SIMPLE_NOTEPAD_PROJECT_FORMATMANAGER_H

#include <QObject>
#include <QTextEdit>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFont>
#include <QColor>

class FormatManager : public QObject {
    Q_OBJECT

public:
    explicit FormatManager(QObject *parent = nullptr);

    ~FormatManager() override = default;

    void setupFormatMenu(QMenu *formatMenu);

    void setupFormatToolbar(QToolBar *toolbar, QTextEdit *editor);

    void setEditor(QTextEdit *editor);

private slots:
    void toggleBold(bool checked);

    void toggleItalic(bool checked);

    void toggleUnderline(bool checked);

    void toggleStrikethrough(bool checked);

    void showFontDialog();

    void showColorDialog();

    void updateFormatActions(const QTextCharFormat &format);

private:
    void setupConnections();

    QTextEdit *editor{nullptr};

    QAction *action_font{nullptr};
    QAction *action_text_color{nullptr};

    QAction *action_bold{nullptr};
    QAction *action_italic{nullptr};
    QAction *action_underline{nullptr};
    QAction *action_strikethrough{nullptr};

    QToolBar *format_toolbar{nullptr};
};

#endif //SIMPLE_NOTEPAD_PROJECT_FORMATMANAGER_H
