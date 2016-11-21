#ifndef GEOMETRYCALCULATIONS_H
#define GEOMETRYCALCULATIONS_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <memory>

class GeometryCalculations
{
public:
    GeometryCalculations();
    ~GeometryCalculations();

    float rectOverlap(const cv::Rect& box1, const cv::Rect& box2);//计算两个矩形的相交度
    float polygonOverlap(const std::vector<cv::Point>& polygon0, const std::vector<cv::Point>& polygon1);//计算两个多边形的相交度
    std::vector<cv::Point> covRectPolygon(const cv::Rect& rect);//将矩形转换为点存储

    //求两条线段的交点
    bool segementIntersection(cv::Point startP0, cv::Point endP0, cv::Point startP1, cv::Point endP1, cv::Point& crossPoint);
    bool segementIntersection(cv::Point2f startP0, cv::Point2f endP0, cv::Point2f startP1, cv::Point2f endP1, cv::Point2f& crossPoint);
    //点是否在多边形内
    bool pointInPolygon(cv::Point point, const std::vector<cv::Point>& polygon);
    bool pointInPolygon(cv::Point2f point, const std::vector<cv::Point2f>& polygon);

    //计算相交的多边形
    std::vector<cv::Point> intersectPolygon(const std::vector<cv::Point>& polygon0, const std::vector<cv::Point>& polygon1);
    //得到多边形的中心
    cv::Point getPolygonCenter(const std::vector<cv::Point>& polygon);
    //根据点的角度对多边形的点排序
    void pointsOrdered(std::vector<cv::Point>& polygon);
    //计算多边形的面积
    float polygonArea(const std::vector<cv::Point>& polygon);

private:

};

#endif // GEOMETRYCALCULATIONS_H
