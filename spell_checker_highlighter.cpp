//
// Created by Shady  on 16/5/26.
//

#include "spell_checker_highlighter.h"

#include <utility>

SpellCheckerHighlighter::SpellCheckerHighlighter(QTextDocument *parent, std::shared_ptr<SpellChecker> checker)
    : QSyntaxHighlighter(parent)
      , spellChecker(std::move(checker)) {
    setupFormats();
}

void SpellCheckerHighlighter::setupFormats() {
    misspelledFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    misspelledFormat.setUnderlineColor(Qt::red);
}

void SpellCheckerHighlighter::setSpellChecker(const std::shared_ptr<SpellChecker> &checker) {
    spellChecker = checker;
    rehighlight();
}

void SpellCheckerHighlighter::rehighlightAll() {
    rehighlight();
}

void SpellCheckerHighlighter::highlightBlock(const QString &text) {
    if (!spellChecker) {
        return;
    }

    static const QRegularExpression wordRegex("\\b[A-Za-z]+\\b");
    QRegularExpressionMatchIterator it = wordRegex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString word = match.captured();

        if (!spellChecker->isSpelledCorrectly(word)) {
            setFormat(match.capturedStart(), match.capturedLength(), misspelledFormat);
        }
    }
}
