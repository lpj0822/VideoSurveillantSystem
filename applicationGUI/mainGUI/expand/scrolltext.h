#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>

class ScrollText : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    explicit ScrollText(QWidget *parent = 0);
    ~ScrollText();
    void setText(const QString &newText);
    QString text() const { return myText; }
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private:
    QString myText;
    int offset;
    int myTimerId;
private:
};


#endif // WIDGET_H
