#ifndef WAREATREE_H
#define WAREATREE_H

#include <QWidget>
#include <QTreeView>
#include <QMouseEvent>
#include <QStandardItemModel>

class WAreaTree : public QTreeView
{
    Q_OBJECT
public:
    explicit WAreaTree(QWidget *parent = 0);
    void updateModel();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:

public slots:

private:
    void performDrag();
    QStandardItemModel *model;
    QPoint startPos;
};

#endif // WAREATREE_H
