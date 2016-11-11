#include "vehiclecountingthread.h"
#include <QDebug>

VehicleCountingThread::VehicleCountingThread()
{
    init();
    std::cout<<"VehicleCountingThread()"<<std::endl;
}

VehicleCountingThread::~VehicleCountingThread()
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
    if(vehicleCouting)
    {
        delete vehicleCouting;
        vehicleCouting=NULL;
    }
    std::cout<<"~VehicleCountingThread()"<<std::endl;
}

void VehicleCountingThread::run()
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
                            int counting=vehicleCouting->detect(readFrame);
                            if(counting<0)
                            {
                                emit signalMessage(errorCodeString.getErrCodeString(counting));
                            }
                            else
                            {
                                emit signalMessage(QString("Vehicle Counting:%1").arg(counting));
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
                    emit signalMessage(errorCodeString.getErrCodeString(errCode));
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
int VehicleCountingThread::startThread(const QString &path)
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
void VehicleCountingThread::stopThread()
{
    isStart=false;
}

//是否打开视频
bool VehicleCountingThread::getIsOpenVideo()
{
    return isOpen;
}

//开始检测
void VehicleCountingThread::startDetect()
{
    vehicleCouting->startTrcaking(true);
    isStartDetection=true;
}

//停止检测
void VehicleCountingThread::stopDetect()
{
    isStartDetection=false;
}

//开始播放
void VehicleCountingThread::startShowVideo()
{
    isReadVideo=true;
}

//停止播放
void VehicleCountingThread::stopShowVideo()
{
    isReadVideo=false;
}

void VehicleCountingThread::initData()
{
    vehicleCouting->initDetectData();
}

void VehicleCountingThread::init()
{
    videoProcess=new VideoProcess();
    vehicleCouting=new VehicleCouting();

    isOpen=false;
    isStartDetection=false;

    isStart=false;//开始运行线程
    isReadVideo=false;//是否读取视频

    frameToimage.load(":/img/play.png");
}
