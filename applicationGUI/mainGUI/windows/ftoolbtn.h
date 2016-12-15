#ifndef FTOOLBTN_H
#define FTOOLBTN_H

#include <QToolButton>

class FToolbtn : public QToolButton
{
    Q_OBJECT
public:
    FToolbtn(const QString &text,const QIcon &icon,QWidget *parent = 0);
};

class WToolbtn : public QToolButton
{
    Q_OBJECT
public:
    WToolbtn(const QIcon &icon, QWidget *parent = 0);
};

#endif // FTOOLBTN_H
