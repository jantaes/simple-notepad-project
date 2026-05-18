//
// Created by Shady  on 17/5/26.
//

#include "line_number.h"
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>

class TextEditHelper : public QTextEdit {
public:
    static void setMargins(QTextEdit *editor, int left, int top, int right, int bottom) {
        static_cast<TextEditHelper *>(editor)->setViewportMargins(left, top, right, bottom);
    }
};

LineNumberMargin::LineNumberMargin(QTextEdit *editor)
    : QWidget(editor)
      , editor(editor) {
    connect(editor->document(), &QTextDocument::blockCountChanged,
            this, [this](int) { updateWidth(); });

    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged,
            this, [this](int) { update(); });

    connect(editor, &QTextEdit::textChanged,
            this, [this]() { update(); });

    connect(editor, &QTextEdit::cursorPositionChanged,
            this, [this]() { update(); });

    updateWidth();
}

QSize LineNumberMargin::sizeHint() const {
    return QSize(calculateWidth(), 0);
}

void LineNumberMargin::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(event->rect(), QColor(Qt::lightGray).lighter(120));

    QTextBlock block = editor->document()->begin();
    int blockNumber = 0;
    int top = (int) editor->document()->documentLayout()->blockBoundingRect(block).translated(
        0, -editor->verticalScrollBar()->value()).top();
    int bottom = top + (int) editor->document()->documentLayout()->blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::darkGray);
            painter.setFont(editor->font());
            painter.drawText(0, top, width() - 4, editor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        blockNumber++;
        top = bottom;
        bottom = top + (int) editor->document()->documentLayout()->blockBoundingRect(block).height();
    }
}

void LineNumberMargin::updateWidth() {
    TextEditHelper::setMargins(editor, calculateWidth(), 0, 0, 0);
}

int LineNumberMargin::calculateWidth() const {
    int digits = 1;
    int max = qMax(1, editor->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    return 10 + editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}
