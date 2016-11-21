#include "vehicleconversedetectthread.h"
#include <iostream>
#include <QDebug>

VehicleConverseDetectThread::VehicleConverseDetectThread()
{
    init();
    std::cout << "VehicleConverseDetectThread()" << std::endl;
}

VehicleConverseDetectThread::~VehicleConverseDetectThread()
{
    if (vehicleConverseDetection)
    {
        delete vehicleConverseDetection;
        vehicleConverseDetection = NULL;
    }
    std::cout << "~VehicleConverseDetectThread()" << std::endl;
}

void VehicleConverseDetectThread::initData()
{
    vehicleConverseDetection->initDetectData();
}

void VehicleConverseDetectThread::detect(const cv::Mat &frame)
{
    int converseArea = vehicleConverseDetection->detect(frame);
    if (converseArea > 0)
    {
        saveTime = QDateTime::currentDateTime();
        isConverse = true;
        emit signalMessage(QString("Vehicle Converse Detection:%1").arg(converseArea), videoProcess->getFramePosition());
    }
    else if (converseArea < 0)
    {
       emit signalMessage(errorCodeString.getErrCodeString(converseArea), videoProcess->getFramePosition());
       if(isConverse)
       {
            int tempDeltaTime = saveTime.secsTo(QDateTime::currentDateTime());
            if (tempDeltaTime > delayTime)
            {
                emit signalSaveVideo();
                isConverse = false;
            }
       }
    }
    else
    {
        if (isConverse)
        {
            int tempDeltaTime = saveTime.secsTo(QDateTime::currentDateTime());
            if (tempDeltaTime > delayTime)
            {
                emit signalSaveVideo();
                isConverse = false;
            }
        }
    }
}

void VehicleConverseDetectThread::init()
{
    vehicleConverseDetection = new VehicleConverseDetection();

    isConverse = false;
    delayTime = 4;//两次区域逆行最短时间
}
