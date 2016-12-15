#include "wtitletableview.h"
#include <QHeaderView>

#pragma execution_character_set("utf-8")

WTitleTableView::WTitleTableView(QAbstractItemModel *model,const QString &str,QWidget *parent) : QWidget(parent)
{
    tableView = new QTableView(this);
    tableView->setModel(model);
    title = new QLabel(str);

    initData();
    initUI();
    initConnect();
}

void WTitleTableView::initData()
{
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->verticalHeader()->setVisible(false);
    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setStretchLastSection(true);

    title->setAlignment(Qt::AlignCenter);
    title->setMinimumHeight(22);

    titleLayout = new QHBoxLayout();
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
}

void WTitleTableView::initUI()
{
    QFrame *frame = new QFrame();
    titleLayout->addWidget(title);
    titleLayout->setStretch(0,9);
    frame->setLayout(titleLayout);
    frame->setFrameShape(QFrame::Box);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(frame);
    mainLayout->addWidget(tableView);

    this->setLayout(mainLayout);
}

void WTitleTableView::initConnect()
{
    ;
}

void WTitleTableView::addWidget(QWidget *w)
{
    titleLayout->addWidget(w);
}

void WTitleTableView::hideColumn(int index)
{
    tableView->setColumnHidden(index, true);
}

void WTitleTableView::setTitle(const QString &str)
{
    title->setText(str);
}
