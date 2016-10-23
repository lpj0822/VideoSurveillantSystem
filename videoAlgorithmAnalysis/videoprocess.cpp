#include "videoprocess.h"
#include <iostream>
#include <QDir>
#include <QDebug>

VideoProcess::VideoProcess(QObject *parent) : QObject(parent)
{
    initData();//初始化数据
    std::cout<<"VideoProcess()"<<std::endl;
}

VideoProcess::~VideoProcess()
{
    closeVideo();
    std::cout<<"~VideoProcess()"<<std::endl;
}

//打开摄像头
int VideoProcess::openVideo(int videoNumber,int width,int height)
{
	if(videoNumber<0)
	{
		isOpenVideo=false;
        qDebug()<<"Unable to open video:"<< videoNumber << endl;
        return -1;
	}
    this->videoNumber=videoNumber;
    captureWidth=width;
    captureHeight=height;
    if(!isOpenVideo)
    {
        capture.open(videoNumber);
        capture.set(CV_CAP_PROP_FRAME_WIDTH,captureWidth);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT,captureHeight);
        if (!capture.isOpened())
        {
            isOpenVideo=false;
            qDebug()<<"Unable to open video:"<< videoNumber << endl;
            return -1;
        }
        isOpenVideo=true;
    }
    else
    {
       closeVideo();
       capture.open(videoNumber);
       capture.set(CV_CAP_PROP_FRAME_WIDTH,captureWidth);
       capture.set(CV_CAP_PROP_FRAME_HEIGHT,captureHeight);
       if (!capture.isOpened())
       {
           isOpenVideo=false;
           qDebug()<<"Unable to open video:"<< videoNumber << endl;
           return -1;
       }
       isOpenVideo=true;
    }
    return 0;
}

//打开视频文件
int VideoProcess::openVideo(const QString& fileName)
{
    if(fileName.trimmed().isEmpty())
    {
		isOpenVideo=false;
        qDebug()<<"Unable to open video:"<< fileName << endl;
        return -1;
    }
    if(!isOpenVideo)
    {
        capture.open(fileName.toStdString().c_str());
        captureWidth=(int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
        captureHeight=(int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
        if (!capture.isOpened())
        {
            isOpenVideo=false;
            qDebug()<<"Unable to open video:"<< fileName << endl;
            return -1;
        }
        isOpenVideo=true;
    }
    else
    {
       closeVideo();
       capture.open(fileName.toStdString().c_str());
       captureWidth=(int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
       captureHeight=(int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
       if (!capture.isOpened())
       {
           isOpenVideo=false;
           qDebug()<<"Unable to open video:"<< fileName << endl;
           return -1;
       }
       isOpenVideo=true;
    }
    return 0;
}

//关闭视频
int VideoProcess::closeVideo()
{
    if(isOpenVideo)
    {
        saveConfig();
        capture.release();
        //destroy GUI windows
        cv::destroyAllWindows();
        isOpenVideo=false;
    }
    return 0;
}

//读取视频帧
int VideoProcess::readFrame(cv::Mat &frame)
{
    if(isOpenVideo)
    {
        if (!capture.read(frame))
        {
            qDebug()<<"Unable to read next frame."<<endl;
            return -2;
        }
    }
    else
    {
        qDebug()<<"video not open!";
        return -1;
    }
    return 0;
}

//设置视频帧的位置
int VideoProcess::setFramePosition(int position)
{
    if(isOpenVideo)
    {
        capture.set(cv::CAP_PROP_POS_FRAMES, position);
        return 0;
    }
    else
    {
        qDebug()<<"video not open!";
        return -1;
    }
}

//获得当前帧位置
int VideoProcess::getFramePosition()
{
    int position=0;
    if(isOpenVideo)
    {
        position=capture.get(cv::CAP_PROP_POS_FRAMES);
    }
    return position;
}

//得到当前视频的位置ms
int VideoProcess::getFramePositionmSec()
{
    int mSec=0;
    if(isOpenVideo)
    {
        mSec=capture.get(cv::CAP_PROP_POS_MSEC);
    }
    return mSec;
}

//获得视频帧率
double VideoProcess::getFrameFPS()
{
    double fps=0;
    if(isOpenVideo)
    {
        fps=capture.get(cv::CAP_PROP_FPS);
    }
    return fps;
}

//视频的总帧数
int VideoProcess::getFrameCount()
{
    int count=0;
    if(isOpenVideo)
    {
        count=capture.get(cv::CAP_PROP_FRAME_COUNT);
    }
    return count;
}

//返回视频编码
int VideoProcess::getVideoFOURCC()
{
    int codec=cv::VideoWriter::fourcc('X','V','I','D');//视频编码格式
    if(isOpenVideo)
    {
        codec=static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));
    }
    return codec;
}

//返回视频大小
cv::Size VideoProcess::getSize()
{
    cv::Size size;
    if(isOpenVideo)
    {
        size.width=(int)capture.get(cv::CAP_PROP_FRAME_WIDTH);;
        size.height=(int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    }
    return size;
}

//是否打开视频
bool VideoProcess::isOpen()
{
    return isOpenVideo;
}

//得到错误码
int VideoProcess::getErrorCode()
{
    return errorCode;
}

//初始化数据
void VideoProcess::initData()
{
    videoNumber=0;
    isOpenVideo=false;

    errorCode=0;

    loadConfig();
}

void VideoProcess::saveConfig()
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
    fs.open("./config/VideoProcess.xml",cv::FileStorage::WRITE,"utf-8");

    cv::write(fs, "captureWidth", captureWidth);
    cv::write(fs, "captureHeight", captureHeight);

    fs.release();
}

void VideoProcess::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/VideoProcess.xml",cv::FileStorage::READ,"utf-8");

    cv::read(fs["captureWidth"], captureWidth,640);
    cv::read(fs["captureHeight"],captureHeight,480);

    fs.release();
}
