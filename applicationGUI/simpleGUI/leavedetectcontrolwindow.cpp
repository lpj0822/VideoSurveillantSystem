#pragma execution_character_set("utf-8")
#include "leavedetectcontrolwindow.h"
#include "leavedetectconfigurewindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

LeaveDetectControlWindow::LeaveDetectControlWindow(QWidget *parent) : QWidget(parent)
{
    initUI();
    initData();
    initConnect();
}

LeaveDetectControlWindow::~LeaveDetectControlWindow()
{
    isOpen = false;
    isStartDetection = false;
    lbPlay->deleteLater();
}

void LeaveDetectControlWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
}

void LeaveDetectControlWindow::slotOpenVideo()
{
    if(!isOpen)
    {
        QString name = QFileDialog::getOpenFileName(this,tr("open video dialog"),".","video files(*.avi *.mp4 *.mpg)");
        if(name.trimmed().isEmpty())
        {
            return;
        }
        path = name;
        lbPlay->showVideo(path);
        if(lbPlay->getIsOpenVideo())
        {
            QFileInfo info1("./config/LeaveDetection.xml");
            if(info1.exists())
            {
                startDetectionButton->setEnabled(true);
            }
            else
            {
                startDetectionButton->setEnabled(false);
            }
            isOpen = true;
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
        isOpen = false;
        isStartDetection = false;
        openVideoButton->setText(tr("打开视频"));
        startDetectionButton->setText(tr("开始离岗检测"));
        configurationButton->setEnabled(false);
        startDetectionButton->setEnabled(false);
    }
}

void LeaveDetectControlWindow::slotConfiguration()
{
    QList<QPolygonF> area;//检测区域
    int maxLeaveTime;//最大离岗时间
    lbPlay->stopVideo();
    LeaveDetectConfigureWindow *window = new LeaveDetectConfigureWindow(lbPlay->getImage(), this);
    int res=window->exec();
    if (res == QDialog::Accepted)
    {
        area = window->getDetectArea();//检测区域
        maxLeaveTime = window->getLeaveMaxTime();//最大离岗时间
        if(area.size() <= 0)
        {
            startDetectionButton->setEnabled(false);
        }
        else
        {
            lbPlay->setConfigParameter(area, maxLeaveTime);
            startDetectionButton->setEnabled(true);
        }
    }
    else
    {
        if(area.size() <= 0)
        {
            startDetectionButton->setEnabled(false);
        }
    }
    window->deleteLater();
    lbPlay->startVideo();
}

void LeaveDetectControlWindow::slotStartDetection()
{
    if(!isStartDetection)
    {
        lbPlay->startDetect();
        startDetectionButton->setText(tr("停止离岗检测"));
        isStartDetection=true;
        configurationButton->setEnabled(false);
    }
    else
    {
        lbPlay->stopDetect();
        startDetectionButton->setText(tr("开始离岗检测"));
        isStartDetection=false;
        configurationButton->setEnabled(true);
    }
}

void LeaveDetectControlWindow::slotVideoMessage(bool isVideoOpen)
{
    if(!isVideoOpen)
    {
        lbPlay->closeShowVideo();
        isOpen=false;
        isStartDetection=false;
        openVideoButton->setText(tr("打开视频"));
        startDetectionButton->setText(tr("开始离岗检测"));
        configurationButton->setEnabled(false);
        startDetectionButton->setEnabled(false);
    }
}

void LeaveDetectControlWindow::slotLeaveMessage(int number)
{
    qDebug() << "Leave area:"<< number;
    commandText->append(QString("当前离岗区域为: %1").arg(number));
}

void LeaveDetectControlWindow::slotSaveVideoMessage(QString path)
{
    qDebug() << "Leave video path:"<< path;
    commandText->append(QString("当前离岗视频保存路径为: %1").arg(path));
}

void LeaveDetectControlWindow::slotClearText()
{
    qDebug()<< commandText->toPlainText() << endl;
    commandText->clear();
}


void LeaveDetectControlWindow::initUI()
{
    openVideoButton=new QPushButton(tr("打开视频"));//打开摄像头按钮

    configurationButton=new QPushButton(tr("配置检测参数"));//配置检测参数选项
    configurationButton->setEnabled(false);

    startDetectionButton=new QPushButton(tr("开始离岗检测"));//开始车辆逆行检测按钮
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

    lbPlay = new LeaveShowVideoWindow(this);
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
    this->setWindowTitle(tr("离岗检测系统"));
}

void LeaveDetectControlWindow::initConnect()
{
    connect(openVideoButton, &QPushButton::clicked, this, &LeaveDetectControlWindow::slotOpenVideo);
    connect(configurationButton, &QPushButton::clicked, this, &LeaveDetectControlWindow::slotConfiguration);
    connect(startDetectionButton, &QPushButton::clicked, this, &LeaveDetectControlWindow::slotStartDetection);
    connect(clearTextButton, &QPushButton::clicked, this, &LeaveDetectControlWindow::slotClearText);

    connect(lbPlay, &LeaveShowVideoWindow::signalLeaveMessage, this, &LeaveDetectControlWindow::slotLeaveMessage);
    connect(lbPlay, &LeaveShowVideoWindow::signalSaveVideoMessage, this, &LeaveDetectControlWindow::slotSaveVideoMessage);
    connect(lbPlay, &LeaveShowVideoWindow::signalVideoMessage, this, &LeaveDetectControlWindow::slotVideoMessage);
}

void LeaveDetectControlWindow::initData()
{
    isOpen=false;
    isStartDetection=false;
}


