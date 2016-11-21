#include "vehiclecountingthread.h"
#include <iostream>
#include <QDebug>

VehicleCountingThread::VehicleCountingThread()
{
    init();
    std::cout << "VehicleCountingThread()" << std::endl;
}

VehicleCountingThread::~VehicleCountingThread()
{
    if (vehicleCouting)
    {
        delete vehicleCouting;
        vehicleCouting = NULL;
    }
    std::cout << "~VehicleCountingThread()" << std::endl;
}

void VehicleCountingThread::initData()
{
    vehicleCouting->initDetectData();
}

void VehicleCountingThread::detect(const cv::Mat &frame)
{
    int counting = vehicleCouting->detect(frame);
    if (counting < 0)
    {
        emit signalMessage(errorCodeString.getErrCodeString(counting));
    }
    else
    {
        emit signalMessage(QString("Vehicle Counting:%1").arg(counting));
    }
}

void VehicleCountingThread::init()
{
    vehicleCouting = new VehicleCouting();
}
