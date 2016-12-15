#include "detectthread.h"
#include <iostream>
#include <QDebug>

DetectThread::DetectThread()
{
    init();
    std::cout << "DetectThread()" << std::endl;
}

DetectThread::~DetectThread()
{
    isReadVideo = false;
    isStartDetection = false;
    isOpen = false;
    isStart = false;
    if(videoProcess)
    {
        videoProcess->closeVideo();
        delete videoProcess;
        videoProcess = NULL;
    }
    std::cout<<"~DetectThread()"<<std::endl;
}

void DetectThread::run()
{
    while(isStart)
    {
        if(isOpen)
        {
            if(isReadVideo)
            {
                int errCode = videoProcess->readFrame(readFrame);
                if(errCode==0)
                {
                    if(!readFrame.empty())
                    {
                        if(isStartDetection)
                        {
                            detect(readFrame);
                        }
                        cv::cvtColor(readFrame, readFrame, cv::COLOR_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
                        frameToimage = QImage((uchar*)(readFrame.data), readFrame.cols, readFrame.rows, QImage::Format_RGB888);
                        emit signalVideoMessage(frameToimage, isOpen);
                    }
                }
                else
                {
                    isStartDetection = false;
                    isReadVideo = false;
                    isOpen = false;
                    isStart = false;
                    emit signalVideoMessage(QImage(":/images/play.png"), isOpen);
                }
                cv::waitKey(1);
                msleep(31);
            }
        }
    }
    videoProcess->closeVideo();
    isStartDetection = false;
    isReadVideo = false;
    isOpen = false;
}

//开始运行线程
int DetectThread::startThread(const QString &path)
{
    int errCode=0;
    if(!isOpen)
    {
        if(path.trimmed().isEmpty())
        {
            qDebug()<<"打开的视频文件路径有误:"<<path<<endl;
            return -1;
        }
        errCode = videoProcess->openVideo(path);
        if(errCode==0)
        {
            isStart = true;
            isOpen = true;
            isReadVideo = true;
        }
        else
        {
            qDebug() << "errorCode:" << errorCodeString.getErrCodeString(errCode)<<endl;
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

//结束运行线程
void DetectThread::stopThread()
{
    isStart = false;
}

//开始播放
void DetectThread::startShowVideo()
{
    isReadVideo = true;
}

//停止播放
void DetectThread::stopShowVideo()
{
    isReadVideo = false;
}

//开始检测
void DetectThread::startDetect()
{
    isStartDetection = true;
}

//停止检测
void DetectThread::stopDetect()
{
    isStartDetection = false;
}

//是否打开视频
bool DetectThread::getIsOpenVideo()
{
    return isOpen;
}

void DetectThread::init()
{
    videoProcess = new VideoProcess();

    isStart = false;//开始运行线程
    isReadVideo = false;//是否读取视频
    isOpen = false;
    isStartDetection = false;

    frameToimage.load(":/images/play.png");
}

