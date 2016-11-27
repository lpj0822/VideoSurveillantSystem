#ifndef POLYGONAREA_H
#define POLYGONAREA_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class PolygonArea
{
public:
    PolygonArea();
    virtual ~PolygonArea();

    void setPolygon(std::vector<cv::Point> points);//设置区域

    std::vector<cv::Point> getPolygon() const;//得到多边形区域
    std::vector<cv::Point> getPolygon1()const;//得到相对于多边形外包矩形坐标的多边形区域
    cv::Rect getPolygonRect() const;//得到多边形的外包矩形

private:

    std::vector<cv::Point> polygon;//多边形办公区
    std::vector<cv::Point> movePolygon;//相对于多边形外包矩形坐标系的多边形区域
    cv::Rect rect;//多边形的外包矩形

    void initData();//初始化数据
};

#endif // POLYGONAREA_H
