#include "countingarea.h"
#include <iostream>

CountingArea::CountingArea()
{
    init();
    std::cout << "CountingArea()" << std::endl;
}

CountingArea::~CountingArea()
{
    std::cout << "~CountingArea()" << std::endl;
}

void CountingArea::setNormalDirection(int direction)
{
    this->normalDirection = direction;
}

int CountingArea::getNormalDirection()
{
    return normalDirection;
}

void CountingArea::setVehicleCouting(int count)
{
    this->vehicleCouting = count;
}

void CountingArea::addVehicleCouting(int count)
{
    this->vehicleCouting += count;
}

int CountingArea::getVehicleCouting()
{
    return vehicleCouting;
}

void CountingArea::setMedianPoint(int position)
{
    this->moveMedianPoint = position;
}

int CountingArea::getMedianPoint()
{
    return moveMedianPoint;
}

//初始化数据
void CountingArea::init()
{
    this->normalDirection = 0;
    this->vehicleCouting = 0;
    this->moveMedianPoint = 0;
}
