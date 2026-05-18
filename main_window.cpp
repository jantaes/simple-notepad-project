//
// Created by Shady  on 11/5/26.
//
#include "main_window.h"
#include "notepad_exception.h"

#include "ui_find_replace_dialog.h"
#include "ui_word_frequency_dialog.h"

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QHeaderView>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTableWidgetItem>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextStream>
#include <QToolBar>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <QTextCursor>
#include <QSettings>
#include <QDebug>

main_window::main_window() {
    setWindowTitle("Notepad");
    resize(800, 600);

    editor = new QTextEdit(this);
    setCentralWidget(editor);
    line_number_area = new LineNumberMargin(editor);
    line_number_area->hide();

    editor->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(editor, &QTextEdit::customContextMenuRequested, this, &main_window::show_spell_check_context_menu);

    transforms.push_back(std::make_unique<uppercase_transform>());
    transforms.push_back(std::make_unique<lowercase_transform>());
    transforms.push_back(std::make_unique<capitalize_transform>());
    transforms.push_back(std::make_unique<sentence_case_transform>());
    transforms.push_back(std::make_unique<swap_case_transform>());

    format_manager = std::make_unique<FormatManager>(this);

    recent_files_manager = std::make_unique<RecentFileManager>(this);
    connect(recent_files_manager.get(), &RecentFileManager::fileSelected,
            this, [this](const QString &filePath) {
                open_file(filePath);
            });

    document_printer = std::make_unique<PrintManager>(this);

    setup_file_menu();
    setup_edit_menu();
    setup_format_menu();
    setup_format_toolbar();
    setup_search_menu();
    setup_tools_menu();
    setup_status_bar();

    initialise_spell_checker();
}

main_window::~main_window() = default;

void main_window::setup_file_menu() {
    auto *file_menu = menuBar()->addMenu("File");

    auto *action_new = file_menu->addAction("New");
    action_new->setShortcut(QKeySequence::New);
    connect(action_new, &QAction::triggered, this, [this] {
        editor->clear();
        current_file.clear();
        update_title();
    });

    file_menu->addSeparator();

    auto *action_open = file_menu->addAction("Open...");
    action_open->setShortcut(QKeySequence::Open);
    connect(action_open, &QAction::triggered, this, [this] {
        open_file();
    });

    auto *action_save = file_menu->addAction("Save");
    action_save->setShortcut(QKeySequence::Save);
    connect(action_save, &QAction::triggered, this, [this] {
        save_file();
    });

    auto *action_save_as = file_menu->addAction("Save As...");
    action_save_as->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(action_save_as, &QAction::triggered, this, [this] {
        save_file_as();
    });

    file_menu->addSeparator();

    auto *action_print = file_menu->addAction("Print...");
    action_print->setShortcut(QKeySequence::Print);
    connect(action_print, &QAction::triggered, this, &main_window::print_document);

    file_menu->addSeparator();

    recent_files_manager->setupMenu(file_menu);

    file_menu->addSeparator();

    auto *action_exit = file_menu->addAction("Exit");
    action_exit->setShortcut(QKeySequence::Quit);
    connect(action_exit, &QAction::triggered, this, [] {
        QApplication::quit();
    });
}

void main_window::setup_edit_menu() {
    auto *edit_menu = menuBar()->addMenu("Edit");

    auto *action_undo = edit_menu->addAction("Undo");
    action_undo->setShortcut(QKeySequence::Undo);
    connect(action_undo, &QAction::triggered, editor, &QTextEdit::undo);

    auto *action_redo = edit_menu->addAction("Redo");
    action_redo->setShortcut(QKeySequence::Redo);
    connect(action_redo, &QAction::triggered, editor, &QTextEdit::redo);

    edit_menu->addSeparator();

    auto *action_cut = edit_menu->addAction("Cut");
    action_cut->setShortcut(QKeySequence::Cut);
    connect(action_cut, &QAction::triggered, editor, &QTextEdit::cut);

    auto *action_copy = edit_menu->addAction("Copy");
    action_copy->setShortcut(QKeySequence::Copy);
    connect(action_copy, &QAction::triggered, editor, &QTextEdit::copy);

    auto *action_paste = edit_menu->addAction("Paste");
    action_paste->setShortcut(QKeySequence::Paste);
    connect(action_paste, &QAction::triggered, editor, &QTextEdit::paste);

    edit_menu->addSeparator();

    auto *action_select_all = edit_menu->addAction("Select All");
    action_select_all->setShortcut(QKeySequence::SelectAll);
    connect(action_select_all, &QAction::triggered, editor, &QTextEdit::selectAll);
}

void main_window::setup_format_menu() {
    auto *format_menu = menuBar()->addMenu("Format");

    format_manager->setupFormatMenu(format_menu);

    format_menu->addSeparator();

    auto *text_case_menu = format_menu->addMenu("Text Case");
    for (const auto &transform: transforms) {
        const auto *action = text_case_menu->addAction(QString::fromStdString(transform->name()));
        connect(action, &QAction::triggered, this, [this, &transform] {
            apply_transform(*transform);
        });
    }
}

void main_window::setup_format_toolbar() {
    auto *toolbar = addToolBar("Format");
    toolbar->setObjectName("FormatToolbar");

    format_manager->setupFormatToolbar(toolbar, editor);
}

void main_window::setup_search_menu() {
    auto *search_menu = menuBar()->addMenu("Search");

    auto *action_find_replace = search_menu->addAction("Find / Replace...");
    action_find_replace->setShortcut(QKeySequence::Find);
    connect(action_find_replace, &QAction::triggered, this, [this] {
        show_find_replace_dialog();
    });
}

void main_window::setup_tools_menu() {
    auto *tools_menu = menuBar()->addMenu("Tools");

    const auto *action_word_freq = tools_menu->addAction("Word Frequency...");
    connect(action_word_freq, &QAction::triggered, this, [this] {
        show_word_frequency();
    });

    tools_menu->addSeparator();

    const auto *action_check_spelling = tools_menu->addAction("Check Spelling...");
    connect(action_check_spelling, &QAction::triggered, this, [this] {
        check_spelling();
    });

    auto *action_line_numbers = tools_menu->addAction("Show Line Numbers");
    action_line_numbers->setCheckable(true);
    action_line_numbers->setChecked(false);
    connect(action_line_numbers, &QAction::toggled, this, &main_window::toggle_line_numbers);
}

void main_window::setup_status_bar() {
    update_status_bar();
    connect(editor, &QTextEdit::textChanged, this, [this] {
        update_status_bar();
    });
}

void main_window::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (line_number_area && line_number_area->isVisible()) {
        QRect cr = contentsRect();
        line_number_area->setGeometry(
            QRect(cr.left(), cr.top(),
                  line_number_area->sizeHint().width(), cr.height())
        );
    }
}

void main_window::toggle_line_numbers(bool checked) {
    if (!line_number_area) return;

    if (checked) {
        line_number_area->show();
        QRect cr = contentsRect();
        line_number_area->setGeometry(
            QRect(cr.left(), cr.top(),
                  line_number_area->sizeHint().width(), cr.height())
        );
    } else {
        line_number_area->hide();
        struct Helper : public QTextEdit {
            static void reset(QTextEdit *e) {
                static_cast<Helper *>(e)->setViewportMargins(0, 0, 0, 0);
            }
        };
        Helper::reset(editor);
    }
}

void main_window::initialise_spell_checker() {
    spell_checker = std::make_shared<SpellChecker>();

    QStringList possiblePaths = {
        "data/words.txt",
        "../data/words.txt",
        "../../data/words.txt",
        QCoreApplication::applicationDirPath() + "/data/words.txt"
    };

    bool loaded = false;
    for (const QString &path: possiblePaths) {
        if (QFile::exists(path)) {
            if (spell_checker->loadDictionary(path)) {
                qDebug() << "Dictionary loaded from:" << path
                        << "(" << spell_checker->getDictionarySize() << "words)";
                loaded = true;
                break;
            }
        }
    }

    if (!loaded) {
        QMessageBox::warning(this, "Spell Checker",
                             "Could not load dictionary file.\n"
                             "Please ensure 'data/words.txt' exists.");
        return;
    }

    highlighter = new SpellCheckerHighlighter(editor->document(), spell_checker);
}

void main_window::show_spell_check_context_menu(const QPoint &pos) {
    QMenu *menu = editor->createStandardContextMenu();

    if (spell_checker && highlighter) {
        QTextCursor cursor = editor->cursorForPosition(pos);
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selectedText().trimmed();

        if (!word.isEmpty() && SpellChecker::isValidWord(word) &&
            !spell_checker->isSpelledCorrectly(word)) {
            menu->addSeparator();

            QStringList suggestions = spell_checker->getSuggestions(word);

            if (!suggestions.isEmpty()) {
                QMenu *suggestionsMenu = menu->addMenu("Spelling Suggestions");

                for (const QString &suggestion: suggestions) {
                    QAction *action = suggestionsMenu->addAction(suggestion);

                    connect(action, &QAction::triggered, [this, word, suggestion]() {
                        QTextCursor cursor = editor->textCursor();
                        cursor.select(QTextCursor::WordUnderCursor);

                        if (cursor.selectedText().trimmed().compare(word, Qt::CaseInsensitive) == 0) {
                            QString replacement = suggestion;

                            if (word == word.toUpper()) {
                                replacement = suggestion.toUpper();
                            } else if (!word.isEmpty() && word[0].isUpper()) {
                                replacement = suggestion;
                                replacement[0] = replacement[0].toUpper();
                            }

                            cursor.insertText(replacement);
                        }
                    });
                }
            } else {
                QAction *noSuggestions = menu->addAction("No suggestions available");
                noSuggestions->setEnabled(false);
            }

            menu->addSeparator();
        }
    }

    menu->exec(editor->viewport()->mapToGlobal(pos));
    delete menu;
}

void main_window::check_spelling() {
    if (!spell_checker || !highlighter) {
        QMessageBox::warning(this, "Spell Checker", "Spell checker is not initialised.");
        return;
    }

    highlighter->rehighlightAll();

    int misspelledCount = 0;
    QStringList misspelledWords;

    QRegularExpression wordRegex("\\b[A-Za-z]+\\b");
    QString text = editor->toPlainText();
    QRegularExpressionMatchIterator it = wordRegex.globalMatch(text);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString word = match.captured();

        if (!spell_checker->isSpelledCorrectly(word)) {
            misspelledCount++;
            if (!misspelledWords.contains(word.toLower())) {
                misspelledWords.append(word.toLower());
            }
        }
    }

    if (misspelledCount == 0) {
        QMessageBox::information(this, "Spell Check Complete", "No misspelled words found.");
    } else {
        QString message = QString("Found %1 misspelled word(s).\n\n").arg(misspelledCount);

        if (misspelledWords.size() <= 10) {
            message += "Misspelled words:\n";
            for (const QString &word: misspelledWords) {
                message += "• " + word + "\n";
            }
        } else {
            message += QString("Showing first 10 of %1 unique misspelled words:\n")
                    .arg(misspelledWords.size());
            for (int i = 0; i < 10; ++i) {
                message += "• " + misspelledWords[i] + "\n";
            }
            message += "...and more.";
        }

        message += "\nRight-click on underlined words for suggestions.";

        QMessageBox::information(this, "Spell Check Results", message);
    }
}

void main_window::update_status_bar() const {
    const QString text = editor->toPlainText();

    int word_count = 0;
    bool in_word = false;
    for (const QChar &ch: text) {
        if (ch.isLetterOrNumber()) {
            if (!in_word) {
                in_word = true;
                ++word_count;
            }
        } else {
            in_word = false;
        }
    }

    const int line_count = text.isEmpty() ? 1 : text.count('\n') + 1;

    statusBar()->showMessage(QString("Words: %1  Lines: %2").arg(word_count).arg(line_count));
}

void main_window::print_document() const {
    document_printer->print(editor);
}

void main_window::apply_transform(const text_transform &transform) const {
    auto cursor = editor->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::Document);
    }
    const int start = cursor.selectionStart();
    const QString selected = cursor.selectedText().replace(QChar::ParagraphSeparator, '\n');
    const std::string original = selected.toStdString();
    const auto result = transform.apply(original);

    cursor.beginEditBlock();
    for (std::size_t i = 0; i < result.size(); ++i) {
        if (original[i] != result[i]) {
            cursor.setPosition(start + static_cast<int>(i));
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
            cursor.insertText(QString(QChar(result[i])), cursor.charFormat());
        }
    }
    cursor.endEditBlock();
}

void main_window::open_file() {
    const auto path = QFileDialog::getOpenFileName(this, "Open File");
    if (path.isEmpty()) {
        return;
    }
    open_file(path);
}

void main_window::open_file(const QString &filePath) {
    try {
        QFile file(filePath);
        if (!file.exists()) {
            throw file_not_found_exception(filePath.toStdString());
        }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw file_read_exception(filePath.toStdString());
        }
        QTextStream in(&file);
        const auto contents = in.readAll();
        if (in.status() != QTextStream::Ok) {
            throw file_read_exception(filePath.toStdString());
        }
        editor->setPlainText(contents);
        current_file = filePath;
        update_title();

        recent_files_manager->addFile(filePath);
    } catch (const notepad_exception &ex) {
        QMessageBox::critical(this, "Error", ex.what());
    }
}

void main_window::save_file() {
    if (current_file.isEmpty()) {
        save_file_as();
        return;
    }
    try {
        QFile file(current_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw file_write_exception(current_file.toStdString());
        }
        QTextStream out(&file);
        out << editor->toPlainText();
    } catch (const notepad_exception &ex) {
        QMessageBox::critical(this, "Error", ex.what());
    }
}

void main_window::save_file_as() {
    const auto path = QFileDialog::getSaveFileName(this, "Save File As");
    if (path.isEmpty()) {
        return;
    }
    current_file = path;
    save_file();
    update_title();

    recent_files_manager->addFile(path);
}

void main_window::update_title() {
    if (current_file.isEmpty()) {
        setWindowTitle("Notepad");
    } else {
        setWindowTitle("Notepad: " + current_file);
    }
}

void main_window::show_find_replace_dialog() {
    if (!find_replace_dlg) {
        find_replace_dlg = new QDialog(this);
        find_replace_ui = std::make_unique<Ui::find_replace_dialog>();
        find_replace_ui->setupUi(find_replace_dlg);

        auto current_flags = [this] {
            auto flags = QTextDocument::FindFlags();
            if (find_replace_ui->case_sensitive_check->isChecked()) {
                flags |= QTextDocument::FindCaseSensitively;
            }
            return flags;
        };

        connect(find_replace_ui->find_next_button, &QPushButton::clicked,
                find_replace_dlg, [this, current_flags] {
                    find_next(find_replace_ui->find_input->text(), current_flags());
                });
        connect(find_replace_ui->replace_button, &QPushButton::clicked,
                find_replace_dlg, [this, current_flags] {
                    replace_current(find_replace_ui->find_input->text(),
                                    find_replace_ui->replace_input->text(), current_flags());
                });
        connect(find_replace_ui->replace_all_button, &QPushButton::clicked,
                find_replace_dlg, [this, current_flags] {
                    replace_all(find_replace_ui->find_input->text(),
                                find_replace_ui->replace_input->text(), current_flags());
                });
        connect(find_replace_ui->close_button, &QPushButton::clicked,
                find_replace_dlg, [this] { find_replace_dlg->hide(); });
    }

    find_replace_dlg->show();
    find_replace_dlg->raise();
    find_replace_dlg->activateWindow();
}

void main_window::find_next(const QString &term, const QTextDocument::FindFlags flags) const {
    if (term.isEmpty()) {
        return;
    }
    auto found = editor->document()->find(term, editor->textCursor(), flags);
    if (found.isNull()) {
        auto from_start = editor->textCursor();
        from_start.movePosition(QTextCursor::Start);
        found = editor->document()->find(term, from_start, flags);
    }
    if (!found.isNull()) {
        editor->setTextCursor(found);
    }
}

void main_window::replace_current(const QString &term, const QString &replacement,
                                  const QTextDocument::FindFlags flags) const {
    if (auto cursor = editor->textCursor(); cursor.hasSelection()) {
        cursor.insertText(replacement);
        editor->setTextCursor(cursor);
    }
    find_next(term, flags);
}

void main_window::replace_all(const QString &term, const QString &replacement,
                              const QTextDocument::FindFlags flags) const {
    if (term.isEmpty()) {
        return;
    }
    auto start_cursor = editor->textCursor();
    start_cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(start_cursor);

    while (true) {
        const auto found = editor->document()->find(term, editor->textCursor(), flags);
        if (found.isNull()) {
            break;
        }
        editor->setTextCursor(found);
        auto c = editor->textCursor();
        c.insertText(replacement);
        editor->setTextCursor(c);
    }
}

void main_window::show_word_frequency() {
    const auto text = editor->toPlainText().toLower().toStdString();

    std::map<std::string, int> freq;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        std::erase_if(word, [](const unsigned char c) {
            return !std::isalpha(c);
        });
        if (!word.empty()) {
            ++freq[word];
        }
    }

    std::vector<std::pair<std::string, int> > sorted_freq(freq.begin(), freq.end());
    std::sort(sorted_freq.begin(), sorted_freq.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });

    auto *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    Ui::word_frequency_dialog ui;
    ui.setupUi(dialog);

    ui.frequency_table->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.frequency_table->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui.frequency_table->setRowCount(static_cast<int>(sorted_freq.size()));
    for (int i = 0; i < static_cast<int>(sorted_freq.size()); ++i) {
        const auto &[w, count] = sorted_freq[i];
        ui.frequency_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(w)));
        auto *count_item = new QTableWidgetItem(QString::number(count));
        count_item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui.frequency_table->setItem(i, 1, count_item);
    }
    ui.frequency_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.frequency_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    dialog->exec();
}
