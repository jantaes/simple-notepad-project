//
// Created by Shady  on 16/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_H
#define SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_H

#include <QString>
#include <QSet>
#include <QVector>

class SpellChecker {
public:
    SpellChecker();

    ~SpellChecker();

    bool loadDictionary(const QString &filepath);

    bool isSpelledCorrectly(const QString &word) const;

    [[nodiscard]] QStringList getSuggestions(const QString &word, int maxSuggestions = 5) const;

    static QString cleanWord(const QString &word);

    static bool isValidWord(const QString &word);

    int getDictionarySize() const { return dictionary.size(); }

private:
    QSet<QString> dictionary;

    static int levenshteinDistance(const QString &s1, const QString &s2);

    bool isLoaded() const;
};

#endif //SIMPLE_NOTEPAD_PROJECT_SPELL_CHECKER_H
