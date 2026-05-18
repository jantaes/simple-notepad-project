//
// Created by Shady  on 17/5/26.
//

#include "recent_file_manager.h"

#include <QFile>
#include <QFileInfo>

RecentFileManager::RecentFileManager(QObject *parent)
    : QObject(parent) {
    loadRecentFiles();
}

RecentFileManager::~RecentFileManager() {
    saveRecentFiles();
}

void RecentFileManager::setupMenu(QMenu *fileMenu) {
    fileMenu->addSeparator();
    recentFilesMenu = fileMenu->addMenu("Recent Files");

    for (int i = 0; i < MAX_RECENT_FILES; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], &QAction::triggered, this, &RecentFileManager::openRecentFile);
        recentFilesMenu->addAction(recentFileActions[i]);
    }

    recentFilesMenu->addSeparator();

    clearRecentAction = new QAction("Clear Recent Files", this);
    clearRecentAction->setVisible(false);
    connect(clearRecentAction, &QAction::triggered, this, &RecentFileManager::clearRecentFiles);
    recentFilesMenu->addAction(clearRecentAction);

    updateRecentFilesMenu();
}

void RecentFileManager::addFile(const QString &filePath) {
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);

    while (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.removeLast();
    }

    saveRecentFiles();
    updateRecentFilesMenu();
}

void RecentFileManager::loadRecentFiles() {
    QSettings settings("SimpleNotepad", "NotepadApp");
    recentFiles = settings.value("recentFiles").toStringList();

    QStringList existingFiles;
    for (const QString &file: recentFiles) {
        if (QFile::exists(file)) {
            existingFiles.append(file);
        }
    }

    if (existingFiles.size() != recentFiles.size()) {
        recentFiles = existingFiles;
        saveRecentFiles();
    }
}

void RecentFileManager::saveRecentFiles() {
    QSettings settings("SimpleNotepad", "NotepadApp");
    settings.setValue("recentFiles", recentFiles);
}

void RecentFileManager::updateRecentFilesMenu() {
    for (int i = 0; i < MAX_RECENT_FILES; ++i) {
        if (i < recentFiles.size()) {
            QFileInfo fileInfo(recentFiles[i]);
            QString text = QString("&%1 %2").arg(i + 1).arg(fileInfo.fileName());
            recentFileActions[i]->setText(text);
            recentFileActions[i]->setToolTip(recentFiles[i]);
            recentFileActions[i]->setData(recentFiles[i]);
            recentFileActions[i]->setVisible(true);
        } else {
            recentFileActions[i]->setVisible(false);
        }
    }

    clearRecentAction->setVisible(!recentFiles.isEmpty());
}

void RecentFileManager::openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString filePath = action->data().toString();
        if (QFile::exists(filePath)) {
            emit fileSelected(filePath);
        } else {
            recentFiles.removeAll(filePath);
            saveRecentFiles();
            updateRecentFilesMenu();
        }
    }
}

void RecentFileManager::clearRecentFiles() {
    recentFiles.clear();
    saveRecentFiles();
    updateRecentFilesMenu();
}
