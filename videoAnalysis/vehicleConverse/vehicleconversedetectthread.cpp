#include "vehicleconversedetectthread.h"
#include <iostream>
#include <QDebug>

VehicleConverseDetectThread::VehicleConverseDetectThread()
{
    init();
    std::cout<<"VehicleConverseDetectThread()"<<std::endl;
}

VehicleConverseDetectThread::~VehicleConverseDetectThread()
{
    isReadVideo=false;
    isStartDetection=false;
    isOpen=false;
    isStart=false;
    if(videoProcess)
    {
        videoProcess->closeVideo();
        delete videoProcess;
        videoProcess=NULL;
    }
    if(vehicleConverseDetection)
    {
        delete vehicleConverseDetection;
        vehicleConverseDetection=NULL;
    }
    std::cout<<"~VehicleConverseDetectThread()"<<std::endl;
}

void VehicleConverseDetectThread::run()
{
    while(isStart)
    {
        if(isOpen)
        {
            if(isReadVideo)
            {
                int errCode=videoProcess->readFrame(readFrame);
                if(errCode==0)
                {
                    if(!readFrame.empty())
                    {
                        if(isStartDetection)
                        {
                             int converseArea=vehicleConverseDetection->detect(readFrame);
                             if(converseArea>0)
                             {
                                 saveTime=QDateTime::currentDateTime();
                                 isConverse=true;
                                 emit signalMessage(QString("Vehicle Converse Detection:%1").arg(converseArea),videoProcess->getFramePosition());
                             }
                             else if(converseArea<0)
                             {
                                emit signalMessage(errorCodeString.getErrCodeString(converseArea),videoProcess->getFramePosition());
                                if(isConverse)
                                {
                                     int tempDeltaTime=saveTime.secsTo(QDateTime::currentDateTime());
                                     if(tempDeltaTime>delayTime)
                                     {
                                         emit signalSaveVideo();
                                         isConverse=false;
                                     }
                                }
                             }
                             else
                             {
                                 if(isConverse)
                                 {
                                     int tempDeltaTime=saveTime.secsTo(QDateTime::currentDateTime());
                                     if(tempDeltaTime>delayTime)
                                     {
                                         emit signalSaveVideo();
                                         isConverse=false;
                                     }
                                 }
                             }
                        }
                        cv::cvtColor(readFrame, readFrame, cv::COLOR_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
                        frameToimage = QImage((uchar*)(readFrame.data), readFrame.cols, readFrame.rows, QImage::Format_RGB888);
                        emit signalVideoMessage(frameToimage,isOpen);
                    }
                }
                else
                {
                    isStart=false;
                    emit signalMessage(errorCodeString.getErrCodeString(errCode),videoProcess->getFramePosition());
                }
                cv::waitKey(1);
                msleep(25);
            }
        }
    }
    isStartDetection=false;
    isReadVideo=false;
    isOpen=false;
    emit signalVideoMessage(QImage(":/img/play.png"),isOpen);
    videoProcess->closeVideo();
}

//开始运行线程
int VehicleConverseDetectThread::startThread(const QString &path)
{
    int errCode=0;
    if(videoProcess)
    {
        if(!isOpen)
        {
            if(path.trimmed().isEmpty())
            {
                qDebug()<<"打开的视频文件路径有误:"<<path<<endl;
                return -1;
            }
            errCode=videoProcess->openVideo(path);
            if(errCode==0)
            {
                isStart=true;
                isOpen=true;
                isReadVideo=true;
            }
            else
            {
                qDebug()<<"errorCode:"<<errorCodeString.getErrCodeString(errCode)<<endl;
                isOpen=false;
            }
            return errCode;
        }
        else
        {
            videoProcess->closeVideo();
            isOpen=false;
            return -1;
        }
    }
    else
    {
        qDebug()<<"ErrorCode:VideoProcess init fail!\n";
        return -1;
    }
}

//结束运行线程
void VehicleConverseDetectThread::stopThread()
{
    isStart=false;
}

//是否打开视频
bool VehicleConverseDetectThread::getIsOpenVideo()
{
    return isOpen;
}

//开始检测
void VehicleConverseDetectThread::startDetect()
{
    vehicleConverseDetection->startTrcaking(true);
    isStartDetection=true;
}

//停止检测
void VehicleConverseDetectThread::stopDetect()
{
    isStartDetection=false;
}

//开始播放
void VehicleConverseDetectThread::startShowVideo()
{
    isReadVideo=true;
}

//停止播放
void VehicleConverseDetectThread::stopShowVideo()
{
    isReadVideo=false;
}

void VehicleConverseDetectThread::initData()
{
    vehicleConverseDetection->initDetectData();
}

void VehicleConverseDetectThread::init()
{
    videoProcess=new VideoProcess();
    vehicleConverseDetection=new VehicleConverseDetection();

    isOpen=false;
    isStartDetection=false;

    isStart=false;//开始运行线程
    isReadVideo=false;//是否读取视频

    frameToimage.load(":/img/play.png");

    isConverse=false;
    delayTime=4;//两次区域入侵最短时间
}
