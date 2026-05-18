//
// Created by Shady  on 16/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_PRINT_MANAGER_H
#define SIMPLE_NOTEPAD_PROJECT_PRINT_MANAGER_H

#include <QObject>
#include <QTextEdit>
#include <QPrinter>

class PrintManager : public QObject {
    Q_OBJECT

public:
    explicit PrintManager(QObject *parent = nullptr);

    ~PrintManager() override = default;

    void print(QTextEdit *textEdit);

private:
    bool showPrintDialog(QPrinter &printer);
};

#endif //SIMPLE_NOTEPAD_PROJECT_PRINT_MANAGER_H
