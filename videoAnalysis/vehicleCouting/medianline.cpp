#include "medianline.h"
#include <iostream>

MedianLine::MedianLine()
{
    std::cout << "MedianLine()" << std::endl;
}

MedianLine::~MedianLine()
{
    std::cout << "~MedianLine()" << std::endl;
}

//根据多边形，得到大概的中位线
QLine MedianLine::getMedianLine(const std::vector<cv::Point>& points,const int& direction)
{
    cv::Point startPoint;
    cv::Point stopPoint;
    getMedianLine(points,direction,startPoint,stopPoint);
    return QLine(startPoint.x,startPoint.y,stopPoint.x,stopPoint.y);
}

//根据多边形，得到大概的中位线
void MedianLine::getMedianLine(const std::vector<cv::Point>& points, const int& direction,cv::Point& point1,cv::Point& point2)
{
    cv::Point tempPoint;
    cv::Rect rect=cv::boundingRect(points);
    point1.x=0;
    point1.y=0;
    point2.x=0;
    point2.y=0;
    switch(direction)
    {
    case 1://上
    case 2://下
        if(rect.tl().x<rect.br().x)
        {
            tempPoint.x=rect.tl().x;
        }
        else
        {
            tempPoint.x=rect.br().x;
        }
        if(rect.tl().y<rect.br().y)
        {
            tempPoint.y=rect.tl().y;
        }
        else
        {
            tempPoint.y=rect.br().y;
        }
        point1.x=tempPoint.x;
        point1.y=tempPoint.y+rect.height/2;
        point2.x=tempPoint.x+rect.width;
        point2.y=point1.y;
        break;
    case 3://左
    case 4://右
        if(rect.tl().x<rect.br().x)
        {
            tempPoint.x=rect.tl().x;
        }
        else
        {
            tempPoint.x=rect.br().x;
        }
        if(rect.tl().y<rect.br().y)
        {
            tempPoint.y=rect.tl().y;
        }
        else
        {
            tempPoint.y=rect.br().y;
        }
        point1.x=tempPoint.x+rect.width/2;
        point1.y=tempPoint.y;
        point2.x=point1.x;
        point2.y=tempPoint.y+rect.height;
        break;
    default:
        break;
    }
}

//根据方向得到比较的中间位置
int MedianLine::getMedianPosition(const std::vector<cv::Point>& points,const int& direction)
{
    cv::Point tempPoint;
    cv::Rect rect=cv::boundingRect(points);
    int medianPosition=0;
    switch(direction)
    {
    case 1://上
    case 2://下
        if(rect.tl().x<rect.br().x)
        {
            tempPoint.x=rect.tl().x;
        }
        else
        {
            tempPoint.x=rect.br().x;
        }
        if(rect.tl().y<rect.br().y)
        {
            tempPoint.y=rect.tl().y;
        }
        else
        {
            tempPoint.y=rect.br().y;
        }
        medianPosition=rect.height/2;
        break;
    case 3://左
    case 4://右
        if(rect.tl().x<rect.br().x)
        {
            tempPoint.x=rect.tl().x;
        }
        else
        {
            tempPoint.x=rect.br().x;
        }
        if(rect.tl().y<rect.br().y)
        {
            tempPoint.y=rect.tl().y;
        }
        else
        {
            tempPoint.y=rect.br().y;
        }
        medianPosition = rect.width/2;
        break;
    default:
        break;
    }
    return medianPosition;
}
