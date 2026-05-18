//
// Created by Shady  on 16/5/26.
//

#include "spell_checker.h"
#include <QFile>
#include <QDebug>
#include <algorithm>

SpellChecker::SpellChecker()
= default;

SpellChecker::~SpellChecker()
= default;

bool SpellChecker::loadDictionary(const QString &filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open dictionary file:" << filepath;
        return false;
    }

    dictionary.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString word = in.readLine().trimmed();
        if (!word.isEmpty() && isValidWord(word)) {
            dictionary.insert(word.toLower());
        }
    }

    file.close();
    qDebug() << "Loaded" << dictionary.size() << "words from dictionary.";
    return !dictionary.isEmpty();
}

QString SpellChecker::cleanWord(const QString &word) {
    QString cleaned;
    for (const QChar &c: word) {
        if (c.isLetter()) {
            cleaned += c.toLower();
        }
    }
    return cleaned;
}

bool SpellChecker::isValidWord(const QString &word) {
    if (word.isEmpty()) return false;

    for (const QChar &c: word) {
        if (!c.isLetter()) {
            return false;
        }
    }
    return true;
}

bool SpellChecker::isSpelledCorrectly(const QString &word) const {
    if (!isLoaded() || word.isEmpty()) {
        return true;
    }

    QString cleaned = cleanWord(word);
    if (cleaned.isEmpty()) {
        return true;
    }

    return dictionary.contains(cleaned);
}

QStringList SpellChecker::getSuggestions(const QString &word, int maxSuggestions) const {
    QVector<QPair<int, QString> > candidates;
    QString cleaned = cleanWord(word);

    if (cleaned.isEmpty() || !isLoaded()) {
        return {};
    }

    for (const QString &dictWord: dictionary) {
        int distance = levenshteinDistance(cleaned, dictWord);
        if (distance <= 3) {
            candidates.append({distance, dictWord});
        }
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                  return a.first < b.first;
              });

    QStringList suggestions;
    for (int i = 0; i < qMin(candidates.size(), maxSuggestions); ++i) {
        suggestions.append(candidates[i].second);
    }

    return suggestions;
}

int SpellChecker::levenshteinDistance(const QString &s1, const QString &s2) {
    const int m = s1.size();
    const int n = s2.size();

    QVector<QVector<int> > dp(m + 1, QVector<int>(n + 1));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + qMin(qMin(dp[i - 1][j],
                                         dp[i][j - 1]),
                                    dp[i - 1][j - 1]);
            }
        }
    }

    return dp[m][n];
}

bool SpellChecker::isLoaded() const {
    return !dictionary.isEmpty();
}
