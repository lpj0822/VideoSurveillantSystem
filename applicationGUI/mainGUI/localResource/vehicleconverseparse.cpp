#include "vehicleconverseparse.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include "vehicleconverseconfig.h"
#include "../windows/weventsearch.h"
#include "../expand/videoplayer.h"

#pragma execution_character_set("utf-8")

VehicleConverseParse::VehicleConverseParse(QWidget *parent) : QDialog(parent)
{
    initData();
    initDetectData();
    initUI();
    initConnect();
}

VehicleConverseParse::~VehicleConverseParse()
{
}

void VehicleConverseParse::initData()
{
    btnConfig = new QPushButton("配置规则");
    btnParse = new QPushButton("开始分析");
    btnConfig->setEnabled(true);
    btnParse->setEnabled(false);

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(200, 200));
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);
}

void VehicleConverseParse::initDetectData()
{
    QFileInfo info1("./config/VehicleConverseDetection.xml");
    if(info1.exists())
    {
        btnParse->setEnabled(true);
    }
    else
    {
        btnParse->setEnabled(false);
    }
    lbPlay = new VehicleConverseShowVideoWindow(this);
}

void VehicleConverseParse::initUI()
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
    mainLayout->setStretch(0, 4);
    mainLayout->setStretch(1, 1);

    this->setLayout(mainLayout);
    this->resize(1200, 600);
    this->setWindowTitle("车辆逆行检测");
}

void VehicleConverseParse::initConnect()
{
    connect(btnConfig, &QPushButton::clicked, this, &VehicleConverseParse::on_BeginConfig);
    connect(btnParse, &QPushButton::clicked, [&](){
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
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &VehicleConverseParse::on_ShowInfoDetails);

    connect(lbPlay, &VehicleConverseShowVideoWindow::signalConverseMessage, this, &VehicleConverseParse::slotConverseMessage);
    connect(lbPlay, &VehicleConverseShowVideoWindow::signalVideoMessage, this, &VehicleConverseParse::slotVideoMessage);
}

void VehicleConverseParse::playVideo(const QString &fileName)
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

void VehicleConverseParse::on_BeginConfig()
{
    lbPlay->stopVideo();
    VehicleConverseConfig *configDialog = new VehicleConverseConfig(this);
    configDialog->updatePreview(lbPlay->getImage());
    if(configDialog->exec()==QDialog::Accepted)
    {
        QList<QPolygonF> polygons = configDialog->getPolygon();
        int direction = configDialog->getDirection();
        QList<int> directions;
        directions.clear();
        //qDebug()<<"polygons:"<<polygons<<" direction:"<<direction;
        for(int loop=0;loop<polygons.size();loop++)
        {
            directions.append(direction);
        }
        if(polygons.size()>0)
        {
            lbPlay->setConfigParameter(polygons,directions);
            btnParse->setEnabled(true);
        }
     }
     configDialog->deleteLater();
     lbPlay->startVideo();
}

void VehicleConverseParse::slotVideoMessage(bool isVideoOpen)
{
    if(!isVideoOpen)
    {
        btnConfig->setEnabled(false);
        btnParse->setEnabled(false);
        btnParse->setText("开始分析");
    }
}


void VehicleConverseParse::slotConverseMessage(int number, QString savePath)
{
    qDebug() << "Converse area:" << number << " Path:" << savePath;

    QImage img = lbPlay->getImage();
    QIcon icon(QPixmap::fromImage(img));

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    QVariant variant100001 = savePath;
    item->setData(100001, variant100001);
    item->setIcon(icon);
    item->setText(QDate::currentDate().toString("yyyy.MM.dd")+" "+QTime::currentTime().toString()+"  车辆逆行");
}

void VehicleConverseParse::on_ShowInfoDetails(QListWidgetItem *item)
{
    VideoPlayer *showInfoDetail = new VideoPlayer(this);
    showInfoDetail->setTitle("车辆逆行检测详细信息");
    showInfoDetail->playVideo(item->data(100001).toString());

    showInfoDetail->exec();
    showInfoDetail->deleteLater();
}
