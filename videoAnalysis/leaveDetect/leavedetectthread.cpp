#include "leavedetectthread.h"
#include <iostream>
#include <QDebug>

LeaveDetectThread::LeaveDetectThread()
{
    init();
    std::cout << "LeaveDetectThread()" << std::endl;
}

LeaveDetectThread::~LeaveDetectThread()
{
    if(leaveDetection)
    {
        delete leaveDetection;
        leaveDetection = NULL;
    }
    std::cout << "~LeaveDetectThread()" << std::endl;
}

void LeaveDetectThread::initData()
{
    leaveDetection->initDetectData();
}

void LeaveDetectThread::detect(const cv::Mat& frame)
{
    int leaveArea = leaveDetection->detect(frame);
    if (leaveArea > 0)
    {
        emit signalMessage(QString("Leave Detection:%1").arg(leaveArea), videoProcess->getFramePosition());
    }
    else if (leaveArea < 0)
    {
        emit signalMessage(errorCodeString.getErrCodeString(leaveArea), videoProcess->getFramePosition());
    }
}

void LeaveDetectThread::init()
{
    leaveDetection = new LeaveDetection();
}

