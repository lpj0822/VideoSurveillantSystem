#include "leavedetectthread.h"
#include <iostream>
#include <QDebug>

LeaveDetectThread::LeaveDetectThread()
{
    init();
    std::cout<<"LeaveDetectThread()"<<std::endl;
}

LeaveDetectThread::~LeaveDetectThread()
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
    if(leaveDetection)
    {
        delete leaveDetection;
        leaveDetection=NULL;
    }
    std::cout<<"~LeaveDetectThread()"<<std::endl;
}

void LeaveDetectThread::run()
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
                             int leaveArea=leaveDetection->detect(readFrame);
                             if(leaveArea>0)
                             {
                                 emit signalMessage(QString("Leave Detection:%1").arg(leaveArea),videoProcess->getFramePositionmSec());
                             }
                             else if(leaveArea<0)
                             {
                                emit signalMessage(errorCodeString.getErrCodeString(leaveArea),videoProcess->getFramePositionmSec());
                             }
                        }
                        cv::cvtColor(readFrame, readFrame, cv::COLOR_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
                        frameToimage = QImage((uchar*)(readFrame.data), readFrame.cols, readFrame.rows, QImage::Format_RGB888);
                        emit signalVideoMessage(frameToimage,isOpen);
                    }
                }
                else
                {
                    isStartDetection=false;
                    isReadVideo=false;
                    isOpen=false;
                    isStart=false;
                    emit signalMessage(errorCodeString.getErrCodeString(errCode),videoProcess->getFramePositionmSec());
                    emit signalVideoMessage(QImage(":/img/play.png"),isOpen);
                }
                cv::waitKey(1);
                msleep(32);
            }
        }
    }
    videoProcess->closeVideo();
}

//开始运行线程
int LeaveDetectThread::startThread(const QString &path)
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
void LeaveDetectThread::stopThread()
{
    isStart=false;
}

//是否打开视频
bool LeaveDetectThread::getIsOpenVideo()
{
    return isOpen;
}

//开始检测
void LeaveDetectThread::startDetect()
{
    isStartDetection=true;
}

//停止检测
void LeaveDetectThread::stopDetect()
{
    isStartDetection=false;
}

//开始播放
void LeaveDetectThread::startShowVideo()
{
    isReadVideo=true;
}

//停止播放
void LeaveDetectThread::stopShowVideo()
{
    isReadVideo=false;
}

void LeaveDetectThread::initData()
{
    leaveDetection->initDetectData();
}

void LeaveDetectThread::init()
{
    videoProcess=new VideoProcess();
    leaveDetection=new LeaveDetection();

    isOpen=false;
    isStartDetection=false;

    isStart=false;//开始运行线程
    isReadVideo=false;//是否读取视频

    frameToimage.load(":/img/play.png");
}

