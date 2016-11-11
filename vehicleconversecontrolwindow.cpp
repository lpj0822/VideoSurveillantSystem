#pragma execution_character_set("utf-8")
#include "vehicleconversecontrolwindow.h"
#include "configurationwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

VehicleConverseControlWindow::VehicleConverseControlWindow(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initData();
    initConnect();
}

VehicleConverseControlWindow::~VehicleConverseControlWindow()
{
    isOpen=false;
    isStartDetection=false;
}

void VehicleConverseControlWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
}

void VehicleConverseControlWindow::slotOpenVideo()
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
                QFileInfo info1("./config/VehicleConverseDetection.xml");
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
            startDetectionButton->setText(tr("开始车辆逆行检测"));
            configurationButton->setEnabled(false);
            startDetectionButton->setEnabled(false);
        }
    }
}

void VehicleConverseControlWindow::slotConfiguration()
{
    lbPlay->stopVideo();
    ConfigurationWindow *window=new ConfigurationWindow(lbPlay->getImage(),this);
    int res=window->exec();
    if(res==QDialog::Accepted)
    {
        area=window->getDetectArea();//检测区域
        areaDirection=window->getDirection();//正常行驶方向
        minConversePointNum=window->getMinConverseNum();//运动路径中至少多少个逆行点

        distThreshold=window->getDistThreshold();//两帧之间目标最大的移动距离
        maxAllowedSkippedFrames=window->getMaxAllowedSkippedFrames();//允许目标消失的最大帧数
        maxTraceLength=window->getMaxTraceLength();//跟踪轨迹的最大长度

        algorithm=window->getAlgorithm();//得到算法类型
        isFilter=window->getIsImageFilter();//是否进行图像滤波
        filterType=window->getImageFilterType();//得到图像滤波器
        minObjectArea=window->getMinObjectArea();//最小能检测目标的面积
        areaCrossValue=window->getAreaCrossValue();//区域相交度最小值

        qDebug()<<"minConversePointNum:"<<minConversePointNum;

        qDebug()<<"distThreshold:"<<distThreshold;
        qDebug()<<"maxAllowedSkippedFrames"<<maxAllowedSkippedFrames;
        qDebug()<<"maxTraceLength:"<<maxTraceLength;

        qDebug()<<"algorithm:"<<algorithm;
        qDebug()<<"isFilter:"<<isFilter;
        qDebug()<<"filterType:"<<filterType;
        qDebug()<<"minObjectArea:"<<minObjectArea;
        qDebug()<<"areaCrossValue:"<<areaCrossValue;

        if(area.size()<=0)
        {
            startDetectionButton->setEnabled(false);
        }
        else
        {
            lbPlay->setConfigParameter(area,areaDirection);
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

void VehicleConverseControlWindow::slotStartDetection()
{
    if(!isStartDetection)
    {
        lbPlay->startDetect();
        startDetectionButton->setText(tr("停止车辆逆行检测"));
        isStartDetection=true;
        configurationButton->setEnabled(false);
    }
    else
    {
        lbPlay->stopDetect();
        startDetectionButton->setText(tr("开始车辆逆行检测"));
        isStartDetection=false;
        configurationButton->setEnabled(true);
    }
}

void VehicleConverseControlWindow::slotVideoMessage(bool isVideoOpen)
{
    if(!isVideoOpen)
    {
        lbPlay->closeShowVideo();
        isOpen=false;
        isStartDetection=false;
        openVideoButton->setText(tr("打开视频"));
        startDetectionButton->setText(tr("开始车辆逆行检测"));
        configurationButton->setEnabled(false);
        startDetectionButton->setEnabled(false);
    }
}

void VehicleConverseControlWindow::slotConverseMessage(int number,QString savePath)
{
    qDebug()<<"Converse area:"<<number<<" Path:"<<savePath;
    commandText->append(QString("车辆逆行行车路径:")+savePath);
}

void VehicleConverseControlWindow::slotClearText()
{
    qDebug()<<commandText->toPlainText()<<endl;
    commandText->clear();
}


void VehicleConverseControlWindow::initUI()
{
    openVideoButton=new QPushButton(tr("打开视频"));//打开摄像头按钮

    configurationButton=new QPushButton(tr("配置检测参数"));//配置检测参数选项
    configurationButton->setEnabled(false);

    startDetectionButton=new QPushButton(tr("开始车辆逆行检测"));//开始车辆逆行检测按钮
    startDetectionButton->setEnabled(false);

    clearTextButton=new QPushButton(tr("清空文本信息"));

    mainLayout=new QVBoxLayout();//主布局

    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setSpacing(30);
    topLayout->addWidget(openVideoButton);
    topLayout->addWidget(configurationButton);
    topLayout->addWidget(startDetectionButton);
    topLayout->addWidget(clearTextButton);

    scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);

    lbPlay = new VehicleConverseShowVideoWindow(this);
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
    this->setMinimumSize(900, 700);
    this->setWindowTitle(tr("车辆逆行检测系统"));
}

void VehicleConverseControlWindow::initConnect()
{
    connect(openVideoButton,&QPushButton::clicked,this,&VehicleConverseControlWindow::slotOpenVideo);
    connect(configurationButton,&QPushButton::clicked,this,&VehicleConverseControlWindow::slotConfiguration);
    connect(startDetectionButton,&QPushButton::clicked,this,&VehicleConverseControlWindow::slotStartDetection);
    connect(clearTextButton,&QPushButton::clicked,this,&VehicleConverseControlWindow::slotClearText);

    connect(lbPlay,&VehicleConverseShowVideoWindow::signalConverseMessage,this,&VehicleConverseControlWindow::slotConverseMessage);
    connect(lbPlay,&VehicleConverseShowVideoWindow::signalVideoMessage,this,&VehicleConverseControlWindow::slotVideoMessage);
}

void VehicleConverseControlWindow::initData()
{

    isOpen=false;
    isStartDetection=false;

    area.clear();
    areaDirection.clear();

    minConversePointNum=6;//运动路径中至少多少个逆行点

    algorithm=0;//算法类型
    isFilter=false;//是否滤波
    filterType=1;//滤波器的类型

    minObjectArea=300;//最小能检测目标的面积
    areaCrossValue=0.1f;//区域相交度最小值

    distThreshold=50.0;//两帧之间目标最大的移动距离
    maxAllowedSkippedFrames=10;//允许目标消失的最大帧数
    maxTraceLength=10;//跟踪轨迹的最大长度
}

void VehicleConverseControlWindow::saveVehicleConverseConfig()
{

}

void VehicleConverseControlWindow::loadVehicleConverseConfig()
{

}

void VehicleConverseControlWindow::saveForegroundConfig()
{
    cv::FileStorage fs;
    QDir makeDir;
    if(!makeDir.exists("./config/"))
    {
        if(!makeDir.mkdir("./config/"))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return;
        }
    }
    fs.open("./config/FrameForeground.xml",cv::FileStorage::WRITE);

    cv::write(fs, "flags", algorithm);
    cv::write(fs, "elementSize", 2);
    cv::write(fs, "enableImageFilter", isFilter);
    cv::write(fs, "filterType", filterType);
    cv::write(fs, "filterSize", 5);

    fs.release();
}

void VehicleConverseControlWindow::loadForegroundConfig()
{
    cv::FileStorage fs;
    fs.open("./config/FrameForeground.xml",cv::FileStorage::READ);

    cv::read(fs["flags"], algorithm,0);
    cv::read(fs["enableImageFilter"], isFilter,false);
    cv::read(fs["filterType"], filterType,1);

    fs.release();
}

void VehicleConverseControlWindow::saveMultipleTrackerConfig()
{
    cv::FileStorage fs;
    QDir makeDir;
    if(!makeDir.exists("./config/"))
    {
        if(!makeDir.mkdir("./config/"))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return;
        }
    }
    fs.open("./config/MultipleTracker.xml",cv::FileStorage::WRITE);

    cv::write(fs, "dist_thres", distThreshold);
    cv::write(fs, "maximum_allowed_skipped_frames", maxAllowedSkippedFrames);
    cv::write(fs, "max_trace_length",maxTraceLength);

    fs.release();
}

void VehicleConverseControlWindow::loadMultipleTrackerConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MultipleTracker.xml",cv::FileStorage::READ);

    cv::read(fs["dist_thres"], distThreshold,60.0);
    cv::read(fs["maximum_allowed_skipped_frames"],maxAllowedSkippedFrames, 10);
    cv::read(fs["max_trace_length"],maxTraceLength, 10);

    fs.release();
}

