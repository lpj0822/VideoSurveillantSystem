#include "leaveparse.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include "leaveconfig.h"
#include "../windows/weventsearch.h"
#include "../expand/videoplayer.h"

#pragma execution_character_set("utf-8")

LeaveParse::LeaveParse(QWidget *parent) : QDialog(parent)
{
    initData();
    initDetectData();
    initUI();
    initConnect();
}

LeaveParse::~LeaveParse()
{
}

void LeaveParse::initData()
{
    btnConfig = new QPushButton("配置规则");
    btnParse = new QPushButton("开始分析");
    btnConfig->setEnabled(false);
    btnParse->setEnabled(false);

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(200, 200));
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);

    saveImg = QImage(":/images/play.png");
}

void LeaveParse::initDetectData()
{
    QFileInfo info1("./config/LeaveDetection.xml");
    if(info1.exists())
    {
        btnParse->setEnabled(true);
    }
    else
    {
        btnParse->setEnabled(false);
    }
    lbPlay = new LeaveShowVideoWindow(this);
}

void LeaveParse::initUI()
{
    QHBoxLayout *leftBelowLayout = new QHBoxLayout();
    leftBelowLayout->addStretch(0);
    leftBelowLayout->addWidget(btnConfig);
    leftBelowLayout->addWidget(btnParse);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(lbPlay);
    leftLayout->addLayout(leftBelowLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(listWidget);
    mainLayout->setStretch(0,4);
    mainLayout->setStretch(1,1);

    this->setLayout(mainLayout);
    this->resize(1200,600);
    this->setWindowTitle("离岗检测");
}

void LeaveParse::initConnect()
{
    connect(btnConfig,&QPushButton::clicked,this, &LeaveParse::on_BeginConfig);
    connect(btnParse,&QPushButton::clicked,[&](){
        if(btnParse->text().contains("开始分析"))
        {
            lbPlay->startDetect();
            btnParse->setText("停止分析");
            btnConfig->setEnabled(false);
         }
         else
         {
            lbPlay->stopDetect();
            btnParse->setText("开始分析");
            btnConfig->setEnabled(true);
         }
    });
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &LeaveParse::on_ShowInfoDetails);

    connect(lbPlay, &LeaveShowVideoWindow::signalLeaveMessage, this, &LeaveParse::slotLeaveMessage);
    connect(lbPlay, &LeaveShowVideoWindow::signalSaveVideoMessage, this, &LeaveParse::slotSaveVideoMessage);
    connect(lbPlay, &LeaveShowVideoWindow::signalVideoMessage, this, &LeaveParse::slotVideoMessage);
}

void LeaveParse::playVideo(const QString &fileName)
{
    path = fileName;
    if(lbPlay)
    {
        lbPlay->showVideo(path);
        if(lbPlay->getIsOpenVideo())
        {
            btnConfig->setEnabled(true);
        }
    }
}

void LeaveParse::on_BeginConfig()
{
    lbPlay->stopVideo();
    LeaveConfig *configDialog = new LeaveConfig(this);
    configDialog->updatePreview(lbPlay->getImage());
    if (configDialog->exec() == QDialog::Accepted)
    {
        QList<QPolygonF> polygons = configDialog->getPolygon();
        int time = configDialog->getTime();
        //qDebug()<<"polygons:"<<polygons<<" time:"<<time;
        if(polygons.size()>0 && time>0)
        {
            lbPlay->setConfigParameter(polygons, time);
            btnParse->setEnabled(true);
        }
     }
     configDialog->deleteLater();
     lbPlay->startVideo();
}

void LeaveParse::slotVideoMessage(bool isVideoOpen)
{
    if(!isVideoOpen)
    {
        btnConfig->setEnabled(false);
        btnParse->setEnabled(false);
        btnParse->setText("开始分析");
    }
}

void LeaveParse::slotLeaveMessage(int number)
{
    qDebug() << "Leave area:" << number;
    saveImg = lbPlay->getImage();
}

void LeaveParse::slotSaveVideoMessage(QString savePath)
{
    qDebug() << "Leave video path:"<< path;
    QIcon icon(QPixmap::fromImage(saveImg));

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    QVariant variant100001 = savePath;
    item->setData(100001, variant100001);
    item->setIcon(icon);
    item->setText(QDate::currentDate().toString("yyyy.MM.dd")+" "+QTime::currentTime().toString()+"  离岗");
}

void LeaveParse::on_ShowInfoDetails(QListWidgetItem *item)
{
    VideoPlayer *showInfoDetail = new VideoPlayer(this);
    showInfoDetail->setTitle("离岗检测详细信息");
    showInfoDetail->playVideo(item->data(100001).toString());

    showInfoDetail->exec();
    showInfoDetail->deleteLater();
}
