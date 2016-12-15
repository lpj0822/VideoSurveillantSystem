#ifndef WEXPAND_H
#define WEXPAND_H

#include <QWidget>
#include <QFrame>
#include <QToolButton>

class WExpand : public QFrame
{
    Q_OBJECT
public:
    explicit WExpand(QWidget *parent = 0);

signals:
    void signalStatusChangeed(bool expanded);

public slots:

private:
    QToolButton *btn;
    bool expanded;
};

#endif // WEXPAND_H
