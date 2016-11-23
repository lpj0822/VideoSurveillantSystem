#include "officearea.h"
#include <iostream>

OfficeArea::OfficeArea()
{
    initData();
    std::cout << "OfficeArea()" << std::endl;
}

OfficeArea::~OfficeArea()
{
    std::cout << "~OfficeArea()" << std::endl;
}

void OfficeArea::setLeaveTime(int initTime)
{
    leaveTime = initTime;
}

int OfficeArea::getLeaveTime()
{
    return leaveTime;
}

//增加离开时间
void OfficeArea::addLeaveTime(int leaveTime)
{
    this->leaveTime += leaveTime;
}

void OfficeArea::setLeaveCount(int count)
{
    this->leaveCount = count;
}

int OfficeArea::getLeaveCount()
{
    return leaveCount;
}

//增加离岗次数
void OfficeArea::addLeaveCount(int count)
{
    this->leaveCount += count;
}

//初始化数据
void OfficeArea::initData()
{
    leaveTime = 0;
    leaveCount = 0;
    isFirstLeave = 0;
}
