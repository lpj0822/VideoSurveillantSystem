#include "drawdetectarea.h"
#include <iostream>

DrawDetectArea::DrawDetectArea(QObject *parent) : QObject(parent)
{
    std::cout << "DrawDetectArea()" << std::endl;
}

DrawDetectArea::~DrawDetectArea()
{
    std::cout << "~DrawDetectArea()" << std::endl;
}

//绘制检测区域
void DrawDetectArea::drawingDetectArea(cv::Mat &inFrame, const std::vector<PolygonArea> &detectArea, cv::Scalar color)
{
    int number = static_cast<int>(detectArea.size());
    for(int loop1=0; loop1<number; loop1++)
    {
        imageProcess.drawPolygon(inFrame, detectArea[loop1].getPolygon(), color);
    }
}
