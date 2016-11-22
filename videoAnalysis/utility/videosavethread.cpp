#include "videosavethread.h"
#include <iostream>
#include <QDir>
#include <QDebug>

VideoSaveThread::VideoSaveThread()
{
    init();
    std::cout<<"VideoSaveThread()"<<std::endl;
}

VideoSaveThread::~VideoSaveThread()
{
    if(video)
    {
        video->closeVideo();
        delete video;
        video=NULL;
    }
    closeVideoWrite();
    std::cout<<"~VideoSaveThread()"<<std::endl;
}

void VideoSaveThread::run()
{
    cv::Mat frame;
    int errCode=0;
    while (isStart)
    {
        if(video->isOpen())
        {
            if(video->getFramePosition() <= stopFrame)
            {
                errCode=video->readFrame(frame);
                if(errCode==0)
                {
                    saveVideo(frame);
                }
                else
                {
                    isStart=false;
                }
            }
            else
            {
                isStart=false;
            }
        }
    }
    video->closeVideo();
    outputVideo.release();
    emit signalVideoSaveFinish(fileName);
}

//打开视频文件
int VideoSaveThread::openVideo(const QString& fileName)
{
    int errCode=0;
    if(video)
    {
        if(!video->isOpen())
        {
            if(fileName.trimmed().isEmpty())
            {
                qDebug()<<"打开的视频文件路径有误:"<<fileName<<endl;
                return -1;
            }
            errCode=video->openVideo(fileName);
            if(errCode>=0)
            {
                size=video->getSize();
                fps=video->getFrameFPS();
            }
            return errCode;
        }
        else
        {
            video->closeVideo();
            if(fileName.trimmed().isEmpty())
            {
                qDebug()<<"打开的视频文件路径有误:"<<fileName<<endl;
                return -1;
            }
            errCode=video->openVideo(fileName);
            return errCode;
        }
    }
    else
    {
        qDebug()<<"ErrorCode:VideoProcess init fail!\n";
        return -1;
    }
}

//初始化保存数据参数
int VideoSaveThread::initSaveVideoData(const QString &fileNameDir, const QString &fileName,long startPos,long intervalSec,bool inv)
{
    int errCode=0;
    QDir makeDir;
    if(!inv)
    {
        startFrame=startPos;
        stopFrame=startPos + intervalSec*fps;
        if(stopFrame>video->getFrameCount())
        {
            stopFrame=video->getFrameCount();
        }
    }
    else
    {
        stopFrame=startPos;
        startFrame=startPos-intervalSec*fps;
        if(startFrame<0)
        {
            startFrame=0;
        }
    }
    errCode=video->setFramePosition(startFrame);
    if(errCode!=0)
    {
        return errCode;
    }
    this->fileName=fileName;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkpath(fileNameDir))
        {
            std::cout << "make dir fail!" << "path:" << fileName.toStdString() << std::endl;
            return -11;
        }
    }

    if(outputVideo.isOpened())
    {
        outputVideo.release();
    }
    if(outputVideo.open(fileName.toStdString().c_str(),codec, fps, size, isColor))
    {
        return 0;
    }
    else
    {
        std::cout << "初始化保存视频对象发生错误" << std::endl;
        return -20;
    }
}

int VideoSaveThread::initSaveVideoData1(const QString &fileNameDir, const QString &fileName,int startSec,int intervalSec,bool inv)
{
    int errCode=0;
    QDir makeDir;
    if(!inv)
    {
        startFrame=startSec*fps;
        stopFrame=(startSec+intervalSec)*fps;
        if(stopFrame>video->getFrameCount())
        {
            stopFrame=video->getFrameCount();
        }
    }
    else
    {
        //startSec=startSec+30;
        stopFrame=startSec*fps;
        startFrame=(startSec-intervalSec)*fps;
        if(startFrame<0)
        {
            startFrame=0;
        }
    }
    errCode=video->setFramePosition(startFrame);
    if(errCode!=0)
    {
        return errCode;
    }
    this->fileName=fileName;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkdir(fileNameDir))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return -11;
        }
    }
    std::cout<<"fileName:"<<fileName.toStdString()<<std::endl;
    if(outputVideo.isOpened())
    {
        outputVideo.release();
    }
    if(outputVideo.open(fileName.toStdString().c_str(),codec,fps,size,isColor))
    {
        return 0;
    }
    else
    {
        std::cout<<"初始化保存视频对象发生错误"<<std::endl;
        return -20;
    }
}

//保存视频文件
int VideoSaveThread::saveVideo(cv::Mat inFrame)
{
    if(inFrame.empty())
    {
        return 0;
    }
    if(outputVideo.isOpened())
    {
        outputVideo.write(inFrame);
        return 0;
    }
    else
    {
        std::cout<<"写入视频数据发生错误"<<std::endl;
        return -21;
    }
}

void VideoSaveThread::startThread()
{
    isStart=true;
}

void VideoSaveThread::stopThread()
{
    isStart=false;
}

//关闭视频写入
void VideoSaveThread::closeVideoWrite()
{
    if(outputVideo.isOpened())
    {
        outputVideo.release();
    }
}

//关闭视频
void VideoSaveThread::closeVideo()
{
    video->closeVideo();
}

void VideoSaveThread::init()
{
    video=new VideoProcess();

    fileName="";

    size=cv::Size(640,480);//保存视频文件的大小
    fps=25;//帧率
    codec=cv::VideoWriter::fourcc('X','V','I','D');//视频编码格式
    isColor=true;//是否是彩色

    startFrame=0;//起始帧
    stopFrame=0;//结束帧

    isStart=false;
}

