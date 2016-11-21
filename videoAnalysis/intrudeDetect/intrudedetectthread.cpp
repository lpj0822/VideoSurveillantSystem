#include "intrudedetectthread.h"
#include <iostream>
#include <QDebug>

IntrudeDetectThread::IntrudeDetectThread()
{
    init();
    std::cout << "IntrudeDetectThread()" << std::endl;
}

IntrudeDetectThread::~IntrudeDetectThread()
{
    if (intrudeDetection)
    {
        delete intrudeDetection;
        intrudeDetection = NULL;
    }
    std::cout << "~IntrudeDetectThread()" << std::endl;
}

void IntrudeDetectThread::initData()
{
    intrudeDetection->initDetectData();
}

void IntrudeDetectThread::detect(const cv::Mat &frame)
{
    int intrudeArea = intrudeDetection->detect(frame);
    if (intrudeArea > 0)
    {
        saveTime = QDateTime::currentDateTime();
        isIntrude = true;
        emit signalMessage(QString("Intrusion Detection:%1").arg(intrudeArea), videoProcess->getFramePosition());
    }
    else if (intrudeArea < 0)
    {
       emit signalMessage(errorCodeString.getErrCodeString(intrudeArea), videoProcess->getFramePosition());
       if (isIntrude)
       {
            int tempDeltaTime = saveTime.secsTo(QDateTime::currentDateTime());
            if (tempDeltaTime > delayTime)
            {
                emit signalSaveVideo();
                isIntrude = false;
            }
       }
    }
    else
    {
        if(isIntrude)
        {
            int tempDeltaTime = saveTime.secsTo(QDateTime::currentDateTime());
            if (tempDeltaTime > delayTime)
            {
                emit signalSaveVideo();
                isIntrude = false;
            }
        }
    }
}

void IntrudeDetectThread::init()
{
    intrudeDetection = new IntrusionDetection();

    isIntrude = false;
    delayTime = 2;//两次区域入侵最短时间
}
