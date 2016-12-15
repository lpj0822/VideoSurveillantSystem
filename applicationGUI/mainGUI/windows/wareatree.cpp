#include "wareatree.h"
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QHeaderView>
#include <QMimeData>
#include <QDrag>
#include "../expand/myhelper.h"

WAreaTree::WAreaTree(QWidget *parent) : QTreeView(parent)
{
    this->setAnimated(true);
    this->setMouseTracking(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //this->setDragDropMode(QAbstractItemView::DragOnly);

    model = new QStandardItemModel(this);
    this->setModel(model);
    this->header()->hide();
    updateModel();
}

void WAreaTree::updateModel()
{
    model->clear();
    QSettings settings(MyHelper::strAreaTree,QSettings::IniFormat);
    auto areasSize = settings.beginReadArray("Areas");
    for(auto i=0;i < areasSize; ++i){
        settings.setArrayIndex(i);
        QStandardItem* itemProject = new QStandardItem(settings.value("areaName").toString());
        model->appendRow(itemProject);
        auto camerasSize = settings.beginReadArray("cameras");
        for(auto j=0;j < camerasSize;j++){
            settings.setArrayIndex(j);
            QStandardItem* itemFolder = new QStandardItem(settings.value("Name").toString());
            itemFolder->setDragEnabled(true);
            itemProject->appendRow(itemFolder);
        }
        settings.endArray();
    }
    settings.endArray();
}

void WAreaTree::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QTreeView::mousePressEvent(event);
}

void WAreaTree::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QTreeView::mouseMoveEvent(event);
}

void WAreaTree::performDrag()
{
    QModelIndex idx = this->currentIndex();
        if(idx.parent().isValid()){
            QMimeData *mimeData = new QMimeData;
            mimeData->setData("Area",model->data(idx.parent()).toByteArray());
            mimeData->setData("Camera",model->data(idx).toByteArray());

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->exec(Qt::CopyAction);
        }
}
