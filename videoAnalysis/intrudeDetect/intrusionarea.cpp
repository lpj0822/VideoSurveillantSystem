#include "intrusionarea.h"
#include <iostream>

IntrusionArea::IntrusionArea()
{
    initData();
    std::cout << "IntrusionArea()" << std::endl;
}

IntrusionArea::~IntrusionArea()
{
    std::cout << "~IntrusionArea()" << std::endl;
}

void IntrusionArea::setIntrusionCount(int count)
{
    this->intrusionCount = count;
}

void IntrusionArea::addIntrusionCount(int count)
{
    this->intrusionCount += count;
}

int IntrusionArea::getIntrusionCount()
{
    return this->intrusionCount;
}

//初始化数据
void IntrusionArea::initData()
{
    this->intrusionCount = 0;
    this->isIntrusion = false;
}

