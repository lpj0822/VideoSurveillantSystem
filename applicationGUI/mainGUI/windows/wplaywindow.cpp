#include "wplaywindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QSplitter>
#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include "../expand/myhelper.h"

#pragma execution_character_set("utf-8")

using namespace QtAV;

PlayCell::PlayCell(QWidget *parent) :
    QFrame(parent),
    Player(nullptr)
{
    lb = new QLabel();
    title = new QLabel();
    lb->setPixmap(QPixmap::fromImage(QImage(":/images/play.png")));
    mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(title);
    mainLayout->addWidget(lb);
    mainLayout->setStretch(0,0);
    mainLayout->setStretch(1,1);
    this->setAcceptDrops(true);
    this->setLayout(mainLayout);
    this->setFrameShape(QFrame::Box);
}

PlayCell::~PlayCell()
{
    qDebug()<<"PlayCell::~PlayCell()";
}

void PlayCell::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
    QWidget::dragEnterEvent(event);
}

void PlayCell::dropEvent(QDropEvent *event)
{
    event->setDropAction(Qt::CopyAction);
    QString areaName = QString(event->mimeData()->data("Area"));
    QString cameraName = QString(event->mimeData()->data("Camera"));

    if(!Player){
        QtAV::Widgets::registerRenderers();
        Renderer = new GLWidgetRenderer2(this);
        if (!Renderer->widget()) {
            QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
            return;
        }

        Player = new AVPlayer(this);
        Player->setRenderer(Renderer);
        mainLayout->removeWidget(lb);
        mainLayout->addWidget(Renderer->widget());

        mainLayout->setStretch(0,0);
        mainLayout->setStretch(1,1);
    }

    title->setText(areaName +"-"+ cameraName);
    Player->play("F:/videos/0.mp4");
    event->accept();
}

void PlayCell::closeEvent(QCloseEvent *event)
{
    if(Player){
        Player->stop();
    }
    QFrame::closeEvent(event);
}

//void PlayCell::paintEvent(QPaintEvent *)//set the backgroud img
//{
//    QPixmap pixmap = QPixmap(":/img/play.png").scaled(this->size());
//    QPainter painter(this);
//    painter.drawPixmap(this->rect(), pixmap);

////    QPainter painter(this);
////    painter.setBrush(Qt::green);
////    painter.drawRect(this->rect());
//}

WPlayWindow::WPlayWindow(QWidget *parent) :
    QFrame(parent)
{
    mainLayout = new QGridLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    readSetting();
    this->setLayout(mainLayout);
//    this->setFrameShape(QFrame::Box);
//    this->setLineWidth(1);
}

void WPlayWindow::setViewType(ViewType type)
{
    writeSetting(type);
    while(!cellGroup.isEmpty()){
        delete cellGroup.takeLast();
    }

    switch (type) {
    case ViewType_1:
        for(int i=0;i<1;i++){
            for(int j=0;j<1;j++){
                PlayCell *lb = new PlayCell();
                mainLayout->addWidget(lb,i,j);
                cellGroup.append(lb);
            }
        }
        break;
    case ViewType_4:
        for(int i=0;i<2;i++){
            for(int j=0;j<2;j++){
                PlayCell *lb = new PlayCell();
                mainLayout->addWidget(lb,i,j);
                cellGroup.append(lb);
            }
        }
        break;
    case ViewType_9:
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                PlayCell *lb = new PlayCell();
                mainLayout->addWidget(lb,i,j);
                cellGroup.append(lb);
            }
        }

        break;
    case ViewType_16:
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                PlayCell *lb = new PlayCell();
                mainLayout->addWidget(lb,i,j);
                cellGroup.append(lb);
            }
        }

        break;
    default:
        break;
    }
}

void WPlayWindow::readSetting()
{
    QSettings settings( MyHelper::strViewType,QSettings::IniFormat);
    ViewType ev = static_cast<ViewType>(settings.value("ViewType").toInt());
    setViewType(ev);
}

void WPlayWindow::writeSetting(ViewType type)
{
    QSettings settings(MyHelper::strViewType,QSettings::IniFormat);
    settings.setValue("ViewType",type);
}
