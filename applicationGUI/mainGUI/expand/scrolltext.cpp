#include "scrolltext.h"
#include <QPainter>
#include <qcoreevent.h>
ScrollText::ScrollText(QWidget *parent) :
    QLabel(parent)
{
    offset = 0;
    myTimerId = 0;
}
ScrollText::~ScrollText()
{
    ;
}

void ScrollText::setText(const QString &newText)
{
    myText = newText;
    update();
    updateGeometry();
}

QSize ScrollText::sizeHint() const
{
    return fontMetrics().size(0, text());
}

void ScrollText::paintEvent(QPaintEvent * )
{
    QPainter painter(this);
//    QPen pen(Qt::yellow, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
//    painter.setPen(pen);
    painter.setPen(Qt::yellow);
    int textWidth = fontMetrics().width(text());
    if (textWidth < 1)
        return;
    int x = -offset;
    while (x < width()) {
        painter.drawText(x, 0, textWidth, height(),
                         Qt::AlignLeft | Qt::AlignVCenter, text());
        x += textWidth;
    }
}

void ScrollText::showEvent(QShowEvent * )
{
    myTimerId = startTimer(30);
}

void ScrollText::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == myTimerId) {
        ++offset;
        if (offset >= fontMetrics().width(text()))
            offset = 0;
        scroll(-1, 0);
    } else {
        QWidget::timerEvent(event);
    }
}

void ScrollText::hideEvent(QHideEvent * )
{
    killTimer(myTimerId);
    myTimerId = 0;
}
