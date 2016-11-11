#include "conversearea.h"
#include <iostream>

ConverseArea::ConverseArea()
{
    PolygonArea::PolygonArea();
    initData();
    std::cout<<"ConverseArea()"<<std::endl;
}

ConverseArea::~ConverseArea()
{
    std::cout<<"~ConverseArea()"<<std::endl;
}

void ConverseArea::setNormalDirection(int direction)
{
    this->normalDirection=direction;
}

int ConverseArea::getNormalDirection()
{
    return normalDirection;
}

//初始化数据
void ConverseArea::initData()
{
    this->normalDirection=0;
    this->isConverse=false;
    this->isFirst=0;
}

