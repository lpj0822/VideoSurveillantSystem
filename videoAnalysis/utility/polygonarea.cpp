#include "polygonarea.h"
#include <iostream>

PolygonArea::PolygonArea()
{
    initData();
    std::cout << "PolygonArea()" << std::endl;
}

PolygonArea::~PolygonArea()
{
    std::cout << "~PolygonArea()" << std::endl;
}

//设置区域
void PolygonArea::setPolygon(std::vector<cv::Point> points)
{
    rect = cv::boundingRect(points);
    this->polygon = points;
    for(int loop=0; loop<(int)points.size(); loop++)
    {
        int x = points[loop].x - rect.x;
        int y = points[loop].y - rect.y;
        this->movePolygon.push_back(cv::Point(x,y));
    }
}

//得到多边形区域
std::vector<cv::Point> PolygonArea::getPolygon() const
{
    return this->polygon;
}

//得到相对于多边形外包矩形坐标的多边形区域
std::vector<cv::Point> PolygonArea::getPolygon1() const
{
    return this->movePolygon;
}

//得到多边形的外包矩形
cv::Rect PolygonArea::getPolygonRect() const
{
    return this->rect;
}

//初始化数据
void PolygonArea::initData()
{
    this->polygon.clear();
    this->movePolygon.clear();
}



