//
// Created by Shady  on 16/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_MAIN_WINDOW_H
#define SIMPLE_NOTEPAD_PROJECT_MAIN_WINDOW_H

#include "text_transform.h"
#include "spell_checker.h"
#include "spell_checker_highlighter.h"
#include "recent_file_manager.h"
#include "print_manager.h"
#include "format_manager.h"
#include "line_number.h"

#include <QDialog>
#include <QMainWindow>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <memory>
#include <vector>

namespace Ui {
    class find_replace_dialog;
    class word_frequency_dialog;
}

class main_window : public QMainWindow {
public:
    main_window();

    ~main_window() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setup_file_menu();

    void setup_edit_menu();

    void setup_format_menu();

    void setup_format_toolbar();

    void setup_search_menu();

    void setup_tools_menu();

    void setup_status_bar();

    void open_file();

    void open_file(const QString &filePath);

    void save_file();

    void save_file_as();

    void update_title();

    void update_status_bar() const;

    void print_document() const;

    void toggle_line_numbers(bool checked);

    void apply_transform(const text_transform &transform) const;

    void show_find_replace_dialog();

    void find_next(const QString &term, QTextDocument::FindFlags flags = QTextDocument::FindFlags()) const;

    void replace_current(const QString &term, const QString &replacement,
                         QTextDocument::FindFlags flags = QTextDocument::FindFlags()) const;

    void replace_all(const QString &term, const QString &replacement,
                     QTextDocument::FindFlags flags = QTextDocument::FindFlags()) const;

    void show_word_frequency();

    void initialise_spell_checker();

    void check_spelling();

    void show_spell_check_context_menu(const QPoint &pos);

    QTextEdit *editor{nullptr};
    QString current_file;
    std::vector<std::unique_ptr<text_transform> > transforms;

    QDialog *find_replace_dlg{nullptr};
    std::unique_ptr<Ui::find_replace_dialog> find_replace_ui;

    std::shared_ptr<SpellChecker> spell_checker;
    SpellCheckerHighlighter *highlighter{nullptr};

    std::unique_ptr<RecentFileManager> recent_files_manager;

    std::unique_ptr<PrintManager> document_printer;

    std::unique_ptr<FormatManager> format_manager;

    LineNumberMargin *line_number_area{nullptr};
};

#endif //SIMPLE_NOTEPAD_PROJECT_MAIN_WINDOW_H
