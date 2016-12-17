#include "videowritethread.h"
#include <iostream>
#include <QDir>
#include <QDebug>

VideoWriteThread::VideoWriteThread()
{
    init();
    std::cout<<"VideoWriteThread()"<<std::endl;
}

VideoWriteThread::~VideoWriteThread()
{
    isStart=false;
    if(video)
    {
        video->closeVideo();
        delete video;
        video=NULL;
    }
    if(outputVideo)
    {
        outputVideo->closeWriteVideo();
        delete outputVideo;
        outputVideo=NULL;
    }
    std::cout<<"~VideoWriteThread()"<<std::endl;
}

void VideoWriteThread::run()
{
    cv::Mat frame;
    int errCode=0;
    while (isStart)
    {
        if(video->isOpen())
        {
            if(video->getFramePosition()<=stopFrame)
            {
                errCode=video->readFrame(frame);
                if(errCode==0)
                {
                    outputVideo->saveVideo(frame);
                }
            }
        }
    }
    video->closeVideo();
    outputVideo->closeWriteVideo();
}

//打开视频文件
int VideoWriteThread::openVideo(const QString& fileName)
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
            if(errCode>=0)
            {
                size=video->getSize();
                fps=video->getFrameFPS();
            }
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
int VideoWriteThread::initSaveVideoData(const QString &fileNameDir, const QString &fileName)
{
    int errCode=0;
    QDir makeDir;
    this->fileName=fileName;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkpath(fileNameDir))
        {
            std::cout<< "make dir fail!" << "fileName:" << fileName.toStdString() << std::endl;
            return -11;
        }
    }
    outputVideo->closeWriteVideo();
    errCode=outputVideo->initSaveVideoData(fileName.toStdString().c_str(),size,fps,codec,isColor);
    return errCode;
}

void VideoWriteThread::startThread()
{
    isStart=true;
}

void VideoWriteThread::stopThread()
{
    isStart=false;
}

//设置保存开始帧
void VideoWriteThread::setStartPos(int pos)
{
    this->startFrame=pos;
    video->setFramePosition(startFrame);
}

//设置保存结束帧
void VideoWriteThread::setStopPos(int pos)
{
    this->stopFrame=pos;
}

//关闭视频写入
void VideoWriteThread::closeVideoWrite()
{
    outputVideo->closeWriteVideo();
}

//关闭视频
void VideoWriteThread::closeVideo()
{
    video->closeVideo();
}

//得到保存路径
QString VideoWriteThread::getSaveFileName()
{
    return this->fileName;
}

//得到写入视频帧的字节数
int VideoWriteThread::getFrameByets()
{
    return outputVideo->getWriteFrameBytes();
}

void VideoWriteThread::init()
{
    video=new VideoProcess();
    outputVideo=new MyVideoWriter();

    fileName = "";

    size=cv::Size(640,480);//保存视频文件的大小
    fps=25;//帧率
    codec=cv::VideoWriter::fourcc('X','V','I','D');//视频编码格式
    isColor=true;//是否是彩色

    startFrame=0;//起始帧
    stopFrame=0;//结束帧

    isStart=false;
}
