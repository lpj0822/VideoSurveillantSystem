#pragma execution_character_set("utf-8")
#include "vehicleconverseshowvideowindow.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QDate>
#include <QTime>

VehicleConverseShowVideoWindow::VehicleConverseShowVideoWindow(QWidget *parent) : QWidget(parent)
{
    initData();
    initConnect();
    std::cout<<"VehicleConverseShowVideoWindow()"<<std::endl;
}

VehicleConverseShowVideoWindow::~VehicleConverseShowVideoWindow()
{
    isOpen=false;
    if(vehicleConverseDetectThread)
    {
        vehicleConverseDetectThread->stopThread();
        vehicleConverseDetectThread->wait();
        delete vehicleConverseDetectThread;
        vehicleConverseDetectThread=NULL;
    }
    if(videoWriteThread)
    {
        videoWriteThread->stopThread();
        videoWriteThread->wait();
        delete videoWriteThread;
        videoWriteThread=NULL;
    }
    std::cout<<"~VehicleConverseShowVideoWindow()"<<std::endl;
}

void VehicleConverseShowVideoWindow::paintEvent(QPaintEvent *e)
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

void VehicleConverseShowVideoWindow::slotVideoImage(QImage image, bool isOpen)
{
    this->isOpen = isOpen;
    currentImage = image;
    this->update();
    if(!isOpen)
    {
        emit signalVideoMessage(isOpen);
    }
}

void VehicleConverseShowVideoWindow::slotMessage(QString message, int pos)
{
    int errCode=0;
    QString str="Vehicle Converse Detection:";
    qDebug()<<message<<" Pos:"<<pos;
    if(message.trimmed().startsWith(str))
    {
        if(isFirstConverse)
        {
            QString filePath = QDir::currentPath()+"/result/vehicleConverse/"+QDate::currentDate().toString("yyyy-MM-dd");
            QString fileName = filePath + "/" + QTime::currentTime().toString("hhmmsszzz") + ".avi";
            errCode=videoWriteThread->openVideo(videoPath);
            if(errCode==0)
            {
                errCode=videoWriteThread->initSaveVideoData(filePath, fileName);
                if(errCode==0)
                {
                    videoWriteThread->setStartPos(pos);
                    videoWriteThread->setStopPos(pos);
                    videoWriteThread->startThread();
                    videoWriteThread->start();
                    isFirstConverse=false;
                }
                else
                {
                    qDebug()<<errorCodeString.getErrCodeString(errCode);
                }
            }
            else
            {
                qDebug()<<errorCodeString.getErrCodeString(errCode);
            }

        }
        else
        {
            videoWriteThread->setStopPos(pos);
        }
        int number=message.trimmed().mid(str.length()).toInt();
        this->detectNumber=number;
    }
}

void VehicleConverseShowVideoWindow::slotSaveVideo()
{
    int minSize = 1024 * 1024;
    videoWriteThread->stopThread();
    videoWriteThread->wait();
    isFirstConverse = true;
    QFileInfo info(videoWriteThread->getSaveFileName());
    if((info.size()/minSize) > 1)
    {
        emit signalConverseMessage(detectNumber, videoWriteThread->getSaveFileName());
    }
    else
    {
        QFile tempFile(videoWriteThread->getSaveFileName());
        if(!tempFile.remove())
        {
            emit signalConverseMessage(detectNumber, videoWriteThread->getSaveFileName());
        }
        tempFile.close();
    }
}

int VehicleConverseShowVideoWindow::showVideo(const QString &path)
{
    int errCode=0;
    this->videoPath = path;
    if(vehicleConverseDetectThread)
    {
        errCode=vehicleConverseDetectThread->startThread(path);
        if(errCode>=0)
        {
            QFileInfo info1("./config/VehicleConverseDetection.xml");
            if(info1.exists())
            {
                loadVehicleConverseConfig();
                vehicleConverseDetectThread->initData();//初始化车辆逆行检测参数
            }
            vehicleConverseDetectThread->start();
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
int VehicleConverseShowVideoWindow::closeShowVideo()
{
    if(vehicleConverseDetectThread)
    {
        vehicleConverseDetectThread->stopThread();
        isOpen = false;
    }
    return 0;
}

//是否打开视频
bool VehicleConverseShowVideoWindow::getIsOpenVideo()
{
    return isOpen;
}

//配置检测参数
void VehicleConverseShowVideoWindow::setConfigParameter(QList<QPolygonF> detectArea, QList<int> directions)
{
    if(detectArea.isEmpty())
    {
        qDebug()<<"区域为空"<<endl;
        return;
    }
    area.clear();
    areaDirection.clear();
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
        areaDirection.push_back(directions[loop]);
    }
    saveVehicleConverseConfig();
    vehicleConverseDetectThread->initData();//初始化车辆逆行检测参数
}

//开始检测
void VehicleConverseShowVideoWindow::startDetect()
{
    vehicleConverseDetectThread->startDetect();
}

//停止检测
void VehicleConverseShowVideoWindow::stopDetect()
{
    vehicleConverseDetectThread->stopDetect();
    videoWriteThread->stopThread();
}

//开始播放
void VehicleConverseShowVideoWindow::startVideo()
{
    vehicleConverseDetectThread->startShowVideo();
}

//停止播放
void VehicleConverseShowVideoWindow::stopVideo()
{
    vehicleConverseDetectThread->stopShowVideo();
    videoWriteThread->stopThread();
}

//得到图片
QImage VehicleConverseShowVideoWindow::getImage()
{
    return currentImage.copy();;
}

//绘制区域
void VehicleConverseShowVideoWindow::drawingArea(QPixmap &pixmap)
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

void VehicleConverseShowVideoWindow::initData()
{

    area.clear();
    copyArea.clear();

    videoPath="";

    vehicleConverseDetectThread=new VehicleConverseDetectThread();//车辆逆行检测
    isOpen=false;
    detectNumber=0;//检测到的区域
    videoWriteThread=new VideoWriteThread();//保存视频

    currentImage=QImage(":/images/play.png");

    //绘图画笔
    myPen.setWidth(2);
    myPen.setStyle(Qt::SolidLine);
    myPen.setColor(Qt::green);

    isFirstConverse=true;
}

void VehicleConverseShowVideoWindow::initConnect()
{
    connect(vehicleConverseDetectThread,&VehicleConverseDetectThread::signalMessage,this,&VehicleConverseShowVideoWindow::slotMessage);
    connect(vehicleConverseDetectThread,&VehicleConverseDetectThread::signalVideoMessage,this,&VehicleConverseShowVideoWindow::slotVideoImage);
    connect(vehicleConverseDetectThread,&VehicleConverseDetectThread::signalSaveVideo,this,&VehicleConverseShowVideoWindow::slotSaveVideo);
}

void VehicleConverseShowVideoWindow::saveVehicleConverseConfig()
{
    cv::FileStorage fs;
    QDir makeDir;
    if(!makeDir.exists("./config/"))
    {
        if(!makeDir.mkdir("./config/"))
        {
            std::cout << "make dir fail!" << std::endl;
            return;
        }
    }
    fs.open("./config/VehicleConverseDetection.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs, "areaDirection", areaDirection);

    for(int loop=0;loop<(int)area.size();loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(),area[loop]);
    }
    fs.release();
}

void VehicleConverseShowVideoWindow::loadVehicleConverseConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    area.clear();
    areaDirection.clear();
    fs.open("./config/VehicleConverseDetection.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["areaDirection"], areaDirection);

    cv::FileNode node = fs["pointsArea0"];
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
