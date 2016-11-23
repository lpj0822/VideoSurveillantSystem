/*
 *Bezier Curve
 *贝塞尔曲线
 */
#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <opencv2/core.hpp>
#include <vector>

class BezierCurve
{
public:
    BezierCurve();
    ~BezierCurve();

    //生成三次贝塞尔曲线点
    std::vector<cv::Point2f> pointBezier3(cv::Point2f startPoint,cv::Point2f ctrlPoint1,cv::Point2f ctrlPoint2,cv::Point2f endPoint,int numPoint);
    //生成二次贝塞尔曲线点
    std::vector<cv::Point2f> pointBezier2(cv::Point2f startPoint,cv::Point2f ctrlPoint,cv::Point2f endPoint,int numPoint);
    //生成一次贝塞尔曲线点
    std::vector<cv::Point2f> pointBezier1(cv::Point2f startPoint,cv::Point2f endPoint,int numPoint);
    //生成二次以上贝塞尔曲线点
    std::vector<cv::Point2f> pointDrawBeziern(const std::vector<cv::Point2f>& ctrlPoint, int n, int numPoint);
    //通过贝塞尔曲线算法生成椭圆弧上的点
    std::vector<cv::Point2f> pointArcToBezier(cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle, int numPoint);
    //通过贝塞尔曲线算法生成椭圆上的点
    std::vector<cv::Point2f> pointEllipseToBezier(cv::Point2f centerPoint, float rx, float ry, int numPoint);

    //将一个椭圆转换为4段三次贝塞尔曲线
    std::vector<cv::Point2f> ellipseToBezier(cv::Point2f centerPoint, float rx, float ry);
    //将一个圆角矩形转换为4段三次贝塞尔曲线
    std::vector<cv::Point2f> roundRectToBezier(cv::Rect2f rect, float rx, float ry);
    //将一个椭圆弧转换为多段三次贝塞尔曲线
    int tAngleArcToBezier(cv::Point2f points[16], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle);
    //去除连续的重复点
    std::vector<cv::Point2f> removeSamePoint(const std::vector<cv::Point2f>& points);

private:
    cv::Point2f Bezier3(cv::Point2f startPoint, cv::Point2f ctrlPoint1, cv::Point2f ctrlPoint2, cv::Point2f endPoint, double t);//三次贝塞尔曲线算法
    cv::Point2f Bezier2(cv::Point2f startPoint, cv::Point2f ctrlPoint, cv::Point2f endPoint,double t);//二次贝塞尔曲线算法
    cv::Point2f Bezier1(cv::Point2f startPoint, cv::Point2f endPoint, double t);//一次贝塞尔曲线算法
    cv::Point2f Beziern(const std::vector<cv::Point2f>& ctrlPoints, int n, double t);//n次贝塞尔曲线算法

    //将一个椭圆弧转换为多段三次贝塞尔曲线
    void angleArcToBezier(cv::Point2f points[4], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle);
    void arcToBezier(cv::Point2f points[4], cv::Point2f centerPoint, float rx, float ry, double start_angle, double sweep_angle);
    int angleArcToBezierPlusSweep(cv::Point2f points[16], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle);
    //将数转换到数值范围[tmin, tmax)内
    double toRange(double value, double tmin, double tmax);
};

#endif // BEZIERCURVE_H
