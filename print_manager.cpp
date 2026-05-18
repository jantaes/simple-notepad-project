//
// Created by Shady  on 16/5/26.
//

#include "print_manager.h"
#include <QPrintDialog>

PrintManager::PrintManager(QObject *parent)
    : QObject(parent) {
}

bool PrintManager::showPrintDialog(QPrinter &printer) {
    QPrintDialog dialog(&printer);
    return dialog.exec() == QDialog::Accepted;
}

void PrintManager::print(QTextEdit *textEdit) {
    if (!textEdit) {
        return;
    }

    QPrinter printer(QPrinter::HighResolution);

    printer.setPageSize(QPageSize::A4);
    printer.setColorMode(QPrinter::Color);

    if (!showPrintDialog(printer)) {
        return;
    }
    textEdit->print(&printer);
}
