//
// Created by Shady  on 17/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_HIGHLIGHTER_H
#define SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <memory>
#include "spell_checker.h"

class SpellCheckerHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SpellCheckerHighlighter(QTextDocument* parent, std::shared_ptr<SpellChecker> checker);
    void setSpellChecker(const std::shared_ptr<SpellChecker> &checker);
    void rehighlightAll();

protected:
    void highlightBlock(const QString& text) override;

private:
    std::shared_ptr<SpellChecker> spellChecker;
    QTextCharFormat misspelledFormat;

    void setupFormats();
};

#endif //SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_HIGHLIGHTER_H