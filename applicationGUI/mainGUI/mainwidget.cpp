#include "mainwidget.h"
#include <QDebug>

#pragma execution_character_set("utf-8")

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    this->resize(800,600);
    Initlabel();
    gridlayout = new QGridLayout(this);
    gridlayout->setSpacing(1);
    gridlayout->setMargin(1);
    this->setLayout(gridlayout);
    InitRightPressMenu();
}

void MainWidget::Initlabel()
{
    for(int i=0; i < 16; i++){
        VideoLabel *label= new VideoLabel(this);
        labellist.append(label);
    }
}

void MainWidget::InitRightPressMenu()
{
    RightPressMenu = new QMenu(this);
    RightPressMenu->addAction("切换到1画面", this, SLOT(ChangeView1()));
    RightPressMenu->addAction("切换到4画面", this, SLOT(ChangeView4()));
    RightPressMenu->addAction("切换到9画面", this, SLOT(ChangeView9()));
    RightPressMenu->addAction("切换到16画面", this, SLOT(ChangeView16()));
}

void MainWidget::ChangeView1()
{
    VideoType = 1;
    RemoveAllLayout();
    gridlayout->addWidget(labellist[0], 0, 0);
    labellist[0]->show();
}

void MainWidget::ChangeView4()
{
    VideoType = 4;
    RemoveAllLayout();
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            gridlayout->addWidget(labellist[i*2 + j], i, j);
            labellist[i*2 + j]->show();
        }
    }
}

void MainWidget::ChangeView9()
{
    VideoType = 9;
    RemoveAllLayout();
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            gridlayout->addWidget(labellist[i*3 + j],i,j);
            labellist[i*3 + j]->show();
        }
    }
}

void MainWidget::ChangeView16()
{
    VideoType = 16;
    RemoveAllLayout();
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            gridlayout->addWidget(labellist[i*4 + j],i,j);
            labellist[i*4 + j]->show();
        }
    }
}

void MainWidget::RemoveAllLayout()
{
    for(int i = 0;i<labellist.count();i++){
        gridlayout->removeWidget(labellist[i]);
        labellist[i]->hide();
    }
}

void MainWidget::resizeEvent(QResizeEvent *e)
{
    for(int i=0;i<16;i++){
        QPixmap pixmap = QPixmap(":/images/sp.png");
        QPixmap tempPix = pixmap.scaled(QSize(e->size().width()/4-1, e->size().height()/4-1), Qt::KeepAspectRatio);
        labellist[i]->label->setPixmap(tempPix);
    }
    ChangeView9();
    QWidget::resizeEvent(e);
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton){
        qDebug()<<"点击右键";
        RightPressMenu->exec(QCursor::pos());
    }
    QWidget::mousePressEvent(e);
}

