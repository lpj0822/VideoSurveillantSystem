#ifndef WMAINPREVIEW_H
#define WMAINPREVIEW_H

#include <QWidget>
#include <QToolBox>
#include <QTreeView>
#include <QStandardItemModel>
#include <QToolButton>
#include "wplaywindow.h"
#include "wareatree.h"

class WMainPreview : public QFrame
{
    Q_OBJECT
public:
    explicit WMainPreview(QWidget *parent = 0);

signals:

public slots:

private:
    void initData();
    void initUI();
    void initConnect();
    void InitLocalConfig();

private:
    QToolBox *toolBox;
    WPlayWindow *palyWindow;

    QToolButton *tbtnView_1;
    QToolButton *tbtnView_4;
    QToolButton *tbtnView_9;
    QToolButton *tbtnView_16;

    WAreaTree *treeArea;
    QStandardItemModel *model;
};

#endif // WMAINPREVIEW_H
