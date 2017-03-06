#pragma execution_character_set("utf-8")
#include "vehiclecountingshowvideowindow.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QDate>
#include <QTime>

VehicleCountingShowVideoWindow::VehicleCountingShowVideoWindow(QWidget *parent) : QWidget(parent)
{
    initData();
    initConnect();
    std::cout<<"VehicleCountingShowVideoWindow()"<<std::endl;
}

VehicleCountingShowVideoWindow::~VehicleCountingShowVideoWindow()
{
    isOpen=false;
    if(vehicleCountingThread)
    {
        vehicleCountingThread->stopThread();
        vehicleCountingThread->wait();
        delete vehicleCountingThread;
        vehicleCountingThread=NULL;
    }
    std::cout<<"~VehicleCountingShowVideoWindow()"<<std::endl;
}

void VehicleCountingShowVideoWindow::paintEvent(QPaintEvent *e)
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

void VehicleCountingShowVideoWindow::slotVideoImage(QImage image,bool isOpen)
{
    this->isOpen=isOpen;
    currentImage=image;
    this->update();
    if(!isOpen)
    {
        emit signalVideoMessage(isOpen);
    }
}

void VehicleCountingShowVideoWindow::slotMessage(QString message)
{
    QString str="Vehicle Counting:";
    if(message.trimmed().startsWith(str))
    {
        int counting=message.trimmed().mid(str.length()).toInt();
        signalCountingMessage(counting);
        //qDebug()<<"Vehicle Counting:"<<counting<<endl;
    }
}

int VehicleCountingShowVideoWindow::showVideo(const QString &path)
{
    int errCode=0;
    this->videoPath = path;
    if(vehicleCountingThread)
    {
        errCode=vehicleCountingThread->startThread(path);
        if(errCode>=0)
        {
            QFileInfo info1("./config/VehicleCouting.xml");
            if(info1.exists())
            {
                loadVehicleCountingConfig();
                vehicleCountingThread->initData();
            }
            vehicleCountingThread->start();
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
int VehicleCountingShowVideoWindow::closeShowVideo()
{
    if(vehicleCountingThread)
    {
        vehicleCountingThread->stopThread();
        isOpen=false;
    }
    return 0;
}

//是否打开视频
bool VehicleCountingShowVideoWindow::getIsOpenVideo()
{
    return isOpen;
}

//配置检测参数
void VehicleCountingShowVideoWindow::setConfigParameter(QList<QPolygonF> detectArea, QList<int> directions)
{
    if(detectArea.isEmpty())
    {
        qDebug()<<"区域为空"<<endl;
        return;
    }
    copyArea=detectArea;
    area.clear();
    areaDirection.clear();
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
        QLine line = medianLine.getMedianLine(tempPoint,directions[loop]);
        copyLine.append(line);
        area.push_back(tempPoint);
        areaDirection.push_back(directions[loop]);
    }
    saveVehicleCountingConfig();
    vehicleCountingThread->initData();
}

//开始检测
void VehicleCountingShowVideoWindow::startDetect()
{
    vehicleCountingThread->startDetect();
}

//停止检测
void VehicleCountingShowVideoWindow::stopDetect()
{
    vehicleCountingThread->stopDetect();
}

//开始播放
void VehicleCountingShowVideoWindow::startVideo()
{
    vehicleCountingThread->startShowVideo();
}

//停止播放
void VehicleCountingShowVideoWindow::stopVideo()
{
    vehicleCountingThread->stopShowVideo();
}

//得到图片
QImage VehicleCountingShowVideoWindow::getImage()
{
    return currentImage.copy();
}

//绘制区域
void VehicleCountingShowVideoWindow::drawingArea(QPixmap &pixmap)
{
    QPainter painter(&pixmap);
    painter.setPen(myPen);
    //设置圆滑边框
    painter.setRenderHint(QPainter::Antialiasing,true);
    for(int loop=0;loop<copyArea.size();loop++)
    {
        painter.drawPolygon(copyArea[loop]);
        painter.drawLine(copyLine[loop]);
    }
    painter.end();
}

void VehicleCountingShowVideoWindow::initData()
{
    copyArea.clear();
    copyLine.clear();
    area.clear();
    areaDirection.clear();

    videoPath="";

    vehicleCountingThread=new VehicleCountingThread();//车流量统计
    isOpen=false;

    currentImage = QImage(":/images/play.png");

    //绘图画笔
    myPen.setWidth(2);
    myPen.setStyle(Qt::SolidLine);
    myPen.setColor(Qt::green);
}

void VehicleCountingShowVideoWindow::initConnect()
{
    connect(vehicleCountingThread, &VehicleCountingThread::signalMessage, this, &VehicleCountingShowVideoWindow::slotMessage);
    connect(vehicleCountingThread, &VehicleCountingThread::signalVideoMessage, this, &VehicleCountingShowVideoWindow::slotVideoImage);
}

void VehicleCountingShowVideoWindow::saveVehicleCountingConfig()
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
    fs.open("./config/VehicleCouting.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs, "areaDirection", areaDirection);

    for(int loop=0;loop<(int)area.size();loop++)
    {
        QString tempName="pointsArea"+QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(),area[loop]);
    }
    fs.release();
}

void VehicleCountingShowVideoWindow::loadVehicleCountingConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    copyArea.clear();
    copyLine.clear();
    area.clear();
    areaDirection.clear();
    fs.open("./config/VehicleCouting.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["areaDirection"], areaDirection);

    cv::FileNode node = fs["pointsArea0"];
    if(node.isNone())
    {
        return;
    }
    cv::FileNodeIterator iterator = node.begin(),iterator_end=node.end();
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
        QLine line=medianLine.getMedianLine(area[loop],areaDirection[loop]);
        copyLine.push_back(line);
        QPolygon tempPolygon(tempData);
        copyArea.push_back(tempPolygon);
    }

    fs.release();
}
