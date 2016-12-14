#pragma execution_character_set("utf-8")
#include "vehiclecountingcontrolwindow.h"
#include "vehiclecountingconfigurewindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

VehicleCountingControlWindow::VehicleCountingControlWindow(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initData();
    initConnect();
}

VehicleCountingControlWindow::~VehicleCountingControlWindow()
{
    isOpen = false;
    isStartDetection = false;
    lbPlay->deleteLater();
}

void VehicleCountingControlWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
}

void VehicleCountingControlWindow::slotOpenVideo()
{
    if(lbPlay)
    {
        if(!isOpen)
        {
            QString name=QFileDialog::getOpenFileName(this,tr("open video dialog"),".","video files(*.avi *.mp4 *.mpg)");
            if(name.trimmed().isEmpty())
            {
                qDebug()<<"打开的视频文件路径有误:"<<name<<endl;
                return;
            }
            path = name;
            lbPlay->showVideo(path);
            if(lbPlay->getIsOpenVideo())
            {
                QFileInfo info1("./config/VehicleCouting.xml");
                if(info1.exists())
                {
                    startDetectionButton->setEnabled(true);
                }
                else
                {
                    startDetectionButton->setEnabled(false);
                }
                isOpen=true;
                openVideoButton->setText(tr("关闭视频"));
                configurationButton->setEnabled(true);
            }
            else
            {
                commandText->append("Error open video!");
            }
        }
        else
        {
            lbPlay->closeShowVideo();
            isOpen=false;
            isStartDetection=false;
            openVideoButton->setText(tr("打开视频"));
            startDetectionButton->setText(tr("开始车流量统计"));
            configurationButton->setEnabled(false);
            startDetectionButton->setEnabled(false);
        }
    }
}

void VehicleCountingControlWindow::slotConfiguration()
{
    QList<QPolygonF> area;//检测区域
    QList<int> areaDirection;//每个区域的正常行驶方向
    lbPlay->stopVideo();
    VehicleCountingConfigureWindow *window=new VehicleCountingConfigureWindow(lbPlay->getImage(), this);
    int res=window->exec();
    if(res==QDialog::Accepted)
    {
        area=window->getDetectArea();//检测区域
        areaDirection=window->getDirection();//正常行驶方向

        if(area.size()<=0)
        {
            startDetectionButton->setEnabled(false);
        }
        else
        {
            lbPlay->setConfigParameter(area, areaDirection);
            startDetectionButton->setEnabled(true);
        }
    }
    else
    {
        if(area.size()<=0)
        {
            startDetectionButton->setEnabled(false);
        }
    }
    window->deleteLater();
    lbPlay->startVideo();
}

void VehicleCountingControlWindow::slotStartDetection()
{
    if(!isStartDetection)
    {
        lbPlay->startDetect();
        startDetectionButton->setText(tr("停止车流量统计"));
        isStartDetection=true;
        configurationButton->setEnabled(false);
    }
    else
    {
        lbPlay->stopDetect();
        startDetectionButton->setText(tr("开始车流量统计"));
        isStartDetection=false;
        configurationButton->setEnabled(true);
    }
}

void VehicleCountingControlWindow::slotVideoMessage(bool isVideoOpen)
{
    if(!isVideoOpen)
    {
        lbPlay->closeShowVideo();
        isOpen=false;
        isStartDetection=false;
        openVideoButton->setText(tr("打开视频"));
        startDetectionButton->setText(tr("开始车流量统计"));
        configurationButton->setEnabled(false);
        startDetectionButton->setEnabled(false);
    }
}

void VehicleCountingControlWindow::slotCountingMessage(int count)
{
    vehicleCountLabel->setText(tr("%1辆").arg(count));
}

void VehicleCountingControlWindow::slotClearText()
{
    qDebug()<<commandText->toPlainText()<<endl;
    commandText->clear();
}


void VehicleCountingControlWindow::initUI()
{
    openVideoButton=new QPushButton(tr("打开视频"));//打开摄像头按钮

    configurationButton=new QPushButton(tr("配置检测参数"));//配置检测参数选项
    configurationButton->setEnabled(false);

    startDetectionButton=new QPushButton(tr("开始车流量统计"));//开始车辆逆行检测按钮
    startDetectionButton->setEnabled(false);

    clearTextButton=new QPushButton(tr("清空文本信息"));

    vehicleCountLabel=new QLabel(tr("0辆"));

    mainLayout=new QVBoxLayout();//主布局

    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setSpacing(30);
    topLayout->addWidget(openVideoButton);
    topLayout->addWidget(configurationButton);
    topLayout->addWidget(startDetectionButton);
    topLayout->addWidget(clearTextButton);
    topLayout->addWidget(vehicleCountLabel);

    scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);

    lbPlay = new VehicleCountingShowVideoWindow(this);
    scrollArea->setWidget(lbPlay);
    scrollArea->setWidgetResizable(true);

    commandText=new QTextBrowser;
    commandText->setFixedHeight(50);
    commandText->setReadOnly(true);

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(commandText);
    this->setLayout(mainLayout);
    //this->setMaximumSize(700,520);
    this->setMinimumSize(900,650);
    this->setWindowTitle(tr("车流量统计系统"));
}

void VehicleCountingControlWindow::initConnect()
{
    connect(openVideoButton,&QPushButton::clicked,this,&VehicleCountingControlWindow::slotOpenVideo);
    connect(configurationButton,&QPushButton::clicked,this,&VehicleCountingControlWindow::slotConfiguration);
    connect(startDetectionButton,&QPushButton::clicked,this,&VehicleCountingControlWindow::slotStartDetection);
    connect(clearTextButton,&QPushButton::clicked,this,&VehicleCountingControlWindow::slotClearText);

    connect(lbPlay,&VehicleCountingShowVideoWindow::signalVideoMessage,this,&VehicleCountingControlWindow::slotVideoMessage);
    connect(lbPlay,&VehicleCountingShowVideoWindow::signalCountingMessage,this,&VehicleCountingControlWindow::slotCountingMessage);
}

void VehicleCountingControlWindow::initData()
{

    isOpen=false;
    isStartDetection=false;
}
