//
// Created by Shady  on 17/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_RECENT_FILE_MANAGER_H
#define SIMPLE_NOTEPAD_PROJECT_RECENT_FILE_MANAGER_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QStringList>
#include <QSettings>

class RecentFileManager : public QObject {
    Q_OBJECT

public:
    explicit RecentFileManager(QObject *parent = nullptr);

    ~RecentFileManager() override;

    void setupMenu(QMenu *fileMenu);

    void addFile(const QString &filePath);

    QMenu *menu() const { return recentFilesMenu; }

signals:
    void fileSelected(const QString &filePath);

private slots:
    void openRecentFile();

    void clearRecentFiles();

private:
    static const int MAX_RECENT_FILES = 5;

    void loadRecentFiles();

    void saveRecentFiles();

    void updateRecentFilesMenu();

    QStringList recentFiles;
    QMenu *recentFilesMenu{nullptr};
    QAction *recentFileActions[MAX_RECENT_FILES]{};
    QAction *clearRecentAction{nullptr};
};

#endif //SIMPLE_NOTEPAD_PROJECT_RECENT_FILE_MANAGER_H
