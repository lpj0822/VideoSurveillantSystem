#pragma execution_character_set("utf-8")
#include "intrudeshowvideowindow.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QDate>
#include <QTime>

IntrudeShowVideoWindow::IntrudeShowVideoWindow(QWidget *parent) : QWidget(parent)
{
    initData();
    initConnect();
    std::cout<<"IntrudeShowVideoWindow()"<<std::endl;
}

IntrudeShowVideoWindow::~IntrudeShowVideoWindow()
{
    isOpen=false;
    if(intrudeDetectThread)
    {
        intrudeDetectThread->stopThread();
        intrudeDetectThread->wait();
        delete intrudeDetectThread;
        intrudeDetectThread=NULL;
    }
    if(videoWriteThread)
    {
        videoWriteThread->stopThread();
        videoWriteThread->wait();
        delete videoWriteThread;
        videoWriteThread=NULL;
    }
    std::cout<<"~IntrudeShowVideoWindow()"<<std::endl;
}

void IntrudeShowVideoWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    if(isOpen)
    {
        QPixmap tempPicture=QPixmap::fromImage(currentImage);
        drawingArea(tempPicture);
        int window_width = width();
        int window_height = height();
        int image_width = tempPicture.width();
        int image_height = tempPicture.height();
        int offset_x = (window_width - image_width) / 2;
        int offset_y = (window_height - image_height) / 2;
        painter.translate(offset_x, offset_y);
        painter.drawPixmap(0,0,tempPicture);
    }
    else
    {
        QPixmap initPicture=QPixmap::fromImage(QImage(":/images/play.png"));
        int window_width = width();
        int window_height = height();
        int image_width = initPicture.width();
        int image_height = initPicture.height();
        int offset_x = (window_width - image_width) / 2;
        int offset_y = (window_height - image_height) / 2;
        painter.translate(offset_x, offset_y);
        painter.drawPixmap(0,0,initPicture);
    }
    painter.end();
}

void IntrudeShowVideoWindow::slotVideoImage(QImage image,bool isOpen)
{
    this->isOpen=isOpen;
    currentImage=image;
    this->update();
    if(!isOpen)
    {
        emit signalVideoMessage(isOpen);
    }
}

void IntrudeShowVideoWindow::slotMessage(QString message,int pos)
{
    int errCode=0;
    QString str="Intrusion Detection:";
    qDebug()<<message<<" Pos:"<<pos;
    if(message.trimmed().startsWith(str))
    {
        if(isFirstIntrude)
        {
            QString filePath = QDir::currentPath() + "/result/intrude/" + QDate::currentDate().toString("yyyy-MM-dd");
            QString fileName = filePath + "/" + QTime::currentTime().toString("hhmmsszzz") + ".avi";
            errCode = videoWriteThread->openVideo(videoPath);
            if(errCode==0)
            {
                errCode = videoWriteThread->initSaveVideoData(filePath, fileName);
                if(errCode == 0)
                {
                    videoWriteThread->setStartPos(pos);
                    videoWriteThread->setStopPos(pos);
                    videoWriteThread->startThread();
                    videoWriteThread->start();
                    isFirstIntrude = false;
                }
                else
                {
                    qDebug() << errorCodeString.getErrCodeString(errCode);
                }
            }
            else
            {
                qDebug() << errorCodeString.getErrCodeString(errCode);
            }

        }
        else
        {
            videoWriteThread->setStopPos(pos);
        }
        int number = message.trimmed().mid(str.length()).toInt();
        this->detectNumber = number;
    }
}

void IntrudeShowVideoWindow::slotSaveVideo()
{
    videoWriteThread->stopThread();
    isFirstIntrude = true;
    emit signalIntrudeMessage(detectNumber, videoWriteThread->getSaveFileName());
}

int IntrudeShowVideoWindow::showVideo(const QString &path)
{
    int errCode=0;
    this->videoPath = path;
    if(intrudeDetectThread)
    {
        errCode=intrudeDetectThread->startThread(path);
        if(errCode>=0)
        {
            QFileInfo info1("./config/IntrusionDetection.xml");
            if(info1.exists())
            {
                loadIntrudesConfig();
                intrudeDetectThread->initData();//初始化离岗检测参数
            }
            intrudeDetectThread->start();
            isOpen=true;
        }
        else
        {
            isOpen=false;
        }
    }
    return 0;
}

//关闭视频
int IntrudeShowVideoWindow::closeShowVideo()
{
    if(intrudeDetectThread)
    {
        intrudeDetectThread->stopThread();
        isOpen=false;
    }
    return 0;
}

//是否打开视频
bool IntrudeShowVideoWindow::getIsOpenVideo()
{
    return isOpen;
}

//配置检测参数
void IntrudeShowVideoWindow::setConfigParameter(QList<QPolygonF> detectArea)
{
    if(detectArea.isEmpty())
    {
        qDebug()<<"区域为空"<<endl;
        return;
    }
    area.clear();
    copyArea=detectArea;
    for(int loop=0;loop<detectArea.size();loop++)
    {
        std::vector<cv::Point> tempPoint;
        QList<QPointF> tempData=detectArea[loop].toList();
        if(tempData.size()<=2)
        {
            qDebug()<<QString("区域%1不合法").arg(loop)<<endl;
            continue;
        }
        for(int loop1=0;loop1<tempData.size();loop1++)
        {
            cv::Point point((int)tempData[loop1].x(),(int)tempData[loop1].y());
            tempPoint.push_back(point);
        }
        area.push_back(tempPoint);
    }
    saveIntrudesConfig();
    intrudeDetectThread->initData();
}

//开始检测
void IntrudeShowVideoWindow::startDetect()
{
    intrudeDetectThread->startDetect();
}

//停止检测
void IntrudeShowVideoWindow::stopDetect()
{
    intrudeDetectThread->stopDetect();
    videoWriteThread->stopThread();
}

//开始播放
void IntrudeShowVideoWindow::startVideo()
{
    intrudeDetectThread->startShowVideo();
}

//停止播放
void IntrudeShowVideoWindow::stopVideo()
{
    intrudeDetectThread->stopShowVideo();
    videoWriteThread->stopThread();
}

//得到图片
QImage IntrudeShowVideoWindow::getImage()
{
    return currentImage.copy();
}

//绘制区域
void IntrudeShowVideoWindow::drawingArea(QPixmap &pixmap)
{
    QPainter painter(&pixmap);
    painter.setPen(myPen);
    //设置圆滑边框
    painter.setRenderHint(QPainter::Antialiasing,true);
    for(int loop=0;loop<copyArea.size();loop++)
    {
        painter.drawPolygon(copyArea[loop]);
    }
    painter.end();
}

void IntrudeShowVideoWindow::initData()
{

    area.clear();
    copyArea.clear();

    videoPath="";

    intrudeDetectThread=new IntrudeDetectThread();//入侵检测
    isOpen=false;
    detectNumber=0;//检测到的区域
    videoWriteThread=new VideoWriteThread();//保存视频

    currentImage = QImage(":/images/play.png");

    //绘图画笔
    myPen.setWidth(2);
    myPen.setStyle(Qt::SolidLine);
    myPen.setColor(Qt::green);

    isFirstIntrude = true;
}

void IntrudeShowVideoWindow::initConnect()
{
    connect(intrudeDetectThread, &IntrudeDetectThread::signalMessage, this, &IntrudeShowVideoWindow::slotMessage);
    connect(intrudeDetectThread, &IntrudeDetectThread::signalVideoMessage, this, &IntrudeShowVideoWindow::slotVideoImage);
    connect(intrudeDetectThread, &IntrudeDetectThread::signalSaveVideo, this, &IntrudeShowVideoWindow::slotSaveVideo);
}

void IntrudeShowVideoWindow::saveIntrudesConfig()
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
    fs.open("./config/IntrusionDetection.xml", cv::FileStorage::WRITE,"utf-8");

    for(int loop=0;loop<(int)area.size();loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(), area[loop]);
    }
    fs.release();
}

void IntrudeShowVideoWindow::loadIntrudesConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    area.clear();
    copyArea.clear();
    fs.open("./config/IntrusionDetection.xml", cv::FileStorage::READ,"utf-8");

    cv::FileNode node=fs["pointsArea0"];
    if(node.isNone())
    {
        return;
    }
    cv::FileNodeIterator iterator=node.begin(),iterator_end=node.end();
    for(int loop=0;iterator!=iterator_end;iterator++,loop++)
    {
        QString tempName="pointsArea"+QString::number(loop);
        if(fs[tempName.toStdString().c_str()].isNone())
            break;
        cv::read(fs[tempName.toStdString().c_str()],tempVector);
        area.push_back(tempVector);
    }

    for(int loop=0;loop<(int)area.size();loop++)
    {
        QVector<QPoint> tempData;
        for(int loop1=0;loop1<(int)area[loop].size();loop1++)
        {
            cv::Point tempPoint=area[loop][loop1];
            tempData.push_back(QPoint(tempPoint.x,tempPoint.y));
        }
        QPolygon tempPolygon(tempData);
        copyArea.push_back(tempPolygon);
    }

    fs.release();
}
