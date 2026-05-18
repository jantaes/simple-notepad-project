//
// Created by Shady  on 17/5/26.
//

#ifndef SIMPLE_NOTEPAD_PROJECT_LINE_NUMBER_H
#define SIMPLE_NOTEPAD_PROJECT_LINE_NUMBER_H

#include <QWidget>
#include <QTextEdit>

class LineNumberMargin : public QWidget {
    Q_OBJECT

public:
    explicit LineNumberMargin(QTextEdit *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateWidth();

    int calculateWidth() const;

    QTextEdit *editor;
};

#endif //SIMPLE_NOTEPAD_PROJECT_LINE_NUMBER_H
