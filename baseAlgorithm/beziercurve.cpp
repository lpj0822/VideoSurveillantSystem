#include "beziercurve.h"
#include <iostream>
#include <cmath>

const double MPI2 = CV_PI / 2;
const double MPI = CV_PI;
const double M2PI = CV_2PI;
const double M = 0.5522847498307933984022516; // 4(sqrt(2)-1)/3

BezierCurve::BezierCurve()
{
    std::cout << "BezierCurve()" << std::endl;
}

BezierCurve::~BezierCurve()
{
    std::cout << "~BezierCurve()" << std::endl;
}

//生成三次贝塞尔曲线点
std::vector<cv::Point2f> BezierCurve::pointBezier3(cv::Point2f startPoint, cv::Point2f ctrlPoint1, cv::Point2f ctrlPoint2, cv::Point2f endPoint, int numPoint)
{
    // t的增量， 可以通过setp大小确定需要保存的曲线上点的个数
    double step = 1.0 / numPoint;
    cv::Point2f point;
    std::vector<cv::Point2f> list;
    for (double t=0.0; t<1.0+0.001; t+=step)
    {
        point = Bezier3(startPoint, ctrlPoint1, ctrlPoint2, endPoint, t);
        list.push_back(point);
    }
    return list;
}

//生成二次贝塞尔曲线点
std::vector<cv::Point2f> BezierCurve::pointBezier2(cv::Point2f startPoint, cv::Point2f ctrlPoint, cv::Point2f endPoint, int numPoint)
{
    // t的增量， 可以通过setp大小确定需要保存的曲线上点的个数
    double step = 1.0 / numPoint;
    cv::Point2f point;
    std::vector<cv::Point2f> list;
    for (double t = 0.0; t < 1.0+0.001; t += step)
    {
         point = Bezier2(startPoint, ctrlPoint, endPoint, t);
         list.push_back(point);
    }
    return list;
}

//生成一次贝塞尔曲线点
std::vector<cv::Point2f> BezierCurve::pointBezier1(cv::Point2f startPoint, cv::Point2f endPoint, int numPoint)
{
    // t的增量， 可以通过setp大小确定需要保存的曲线上点的个数
    double step = 1.0 / numPoint;
    cv::Point2f point;
    std::vector<cv::Point2f> list;
    for (double t = 0.0; t < 1.0+0.001; t += step)
    {
         point = Bezier1(startPoint, endPoint, t);
         list.push_back(point);
    }
    return list;
}

//生成二次以上贝塞尔曲线点
std::vector<cv::Point2f> BezierCurve::pointDrawBeziern(const std::vector<cv::Point2f>& ctrlPoint, int n, int numPoint)
{
    CV_Assert(n >= 2);
    double step = 1.0 / numPoint;
    std::vector<cv::Point2f> list;
    cv::Point2f point;
    for(double t = 0.0; t < 1.0+0.001; t += step) //调整参数t,计算贝塞尔曲线上的点的坐标,t即为上述u
    {
          point = Beziern(ctrlPoint, n, t);
          list.push_back(point);
    }
    return list;
}

//通过贝塞尔曲线算法生成椭圆弧上的点
std::vector<cv::Point2f> BezierCurve::pointArcToBezier(cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle, int numPoint)
{
    std::vector<cv::Point2f> list;
    std::vector<cv::Point2f> blist;
    cv::Point2f points[16];
    int n = 0;
    int loop = 0;
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    double endAngle = 0.0;
    n = tAngleArcToBezier(points, centerPoint, rx, ry, startAngle, sweepAngle);
    // 计算第一段椭圆弧的终止角度
    if (startAngle < MPI2)
    {
        // +Y
        endAngle = MPI2;
    }
    else if (startAngle < MPI)
    {
        // -X
        endAngle = MPI;
    }
    else if (startAngle < 3.0 * MPI2)
    {
        // -Y
        endAngle = 3.0 * MPI2;
    }
    else
    {
        // +X
        endAngle = M2PI;
    }
    if(n<4)
    {
        return list;
    }
    if(n == 4)
    {
        num1 = numPoint;
    }
    else if(n == 7)
    {
        num1 = static_cast<int>((endAngle - startAngle) / fabs(sweepAngle) * numPoint);
        num2 = numPoint - num1;
    }
    else if(n == 10)
    {
        num1 = static_cast<int>((endAngle - startAngle) / fabs(sweepAngle)*numPoint);
        num2 = static_cast<int>(MPI2 / fabs(sweepAngle) * numPoint);
        num3 = numPoint - num1 - num2;
    }
    else if(n == 13)
    {
        num1 = static_cast<int>((endAngle - startAngle) / fabs(sweepAngle) * numPoint);
        num2 = static_cast<int>(MPI2 / fabs(sweepAngle) * numPoint);
        num3 = static_cast<int>(MPI2 / fabs(sweepAngle) * numPoint);
        num4 = numPoint - num1 - num2 - num3;
    }
    else
    {
        return list;
    }
    loop = 1;
    while(loop < n)
    {
        if(loop < 4)
        {
             blist = pointBezier3(points[loop-1], points[loop], points[loop+1], points[loop+2], num1);
        }
        else if(loop < 7)
        {
            blist = pointBezier3(points[loop-1], points[loop], points[loop+1], points[loop+2], num2);
        }
        else if(loop < 10)
        {
             blist = pointBezier3(points[loop-1], points[loop], points[loop+1], points[loop+2], num3);
        }
        else if(loop < 13)
        {
             blist = pointBezier3(points[loop-1], points[loop], points[loop+1], points[loop+2], num4);
        }
        for(int loop1 = 0; loop1<blist.size(); loop1++)
        {
            list.push_back(blist[loop1]);
        }
        blist.clear();
        loop = loop + 3;
    }
    return list;
}

//通过贝塞尔曲线算法生成椭圆上的点
std::vector<cv::Point2f> BezierCurve::pointEllipseToBezier(cv::Point2f centerPoint, float rx, float ry,int numPoint)
{
    std::vector<cv::Point2f> list;
    std::vector<cv::Point2f> ctrlList;
    std::vector<cv::Point2f> blist;
    int num = 0;
    ctrlList = ellipseToBezier(centerPoint,rx,ry);
    if (numPoint % 4 != 0)
    {
        num = static_cast<int>((numPoint + numPoint % 4) * 1.0f / 4.0f);
    }
    else
    {
        num = static_cast<int>(numPoint * 1.0f / 4.0f);
    }
    if(ctrlList.size() < 13)
    {
        return list;
    }
    blist = pointBezier3(ctrlList[0], ctrlList[1] ,ctrlList[2], ctrlList[3], num);
    for(int loop=0; loop<blist.size(); loop++)
    {
        list.push_back(blist[loop]);
    }
    blist.clear();
    blist = pointBezier3(ctrlList[3], ctrlList[4], ctrlList[5], ctrlList[6], num);
    for(int loop=0; loop<blist.size(); loop++)
    {
        list.push_back(blist[loop]);
    }
    blist.clear();
    blist = pointBezier3(ctrlList[6], ctrlList[7], ctrlList[8], ctrlList[9], num);
    for(int loop=0; loop<blist.size(); loop++)
    {
        list.push_back(blist[loop]);
    }
    blist.clear();
    blist = pointBezier3(ctrlList[9], ctrlList[10], ctrlList[11], ctrlList[12], num);
    for(int loop=0; loop<blist.size();loop++)
    {
        list.push_back(blist[loop]);
    }
    return list;
}

/*
 * 将一个椭圆转换为4段三次贝塞尔曲线4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，
 * 第一个点和最后一个点重合于+X轴上点(rx, 0).
 * param:
   in: centerPoint 椭圆心
   in: rx 半长轴的长度
   in: ry 半短轴的长度
 *retrun:
   out: 贝塞尔曲线的控制点，13个点
*/
std::vector<cv::Point2f> BezierCurve::ellipseToBezier(cv::Point2f centerPoint, float rx, float ry)
{
    float dx = static_cast<float>(rx * M);
    float dy = static_cast<float>(ry * M);
    std::vector<cv::Point2f> list;

    list.push_back(cv::Point2f(centerPoint.x + rx, centerPoint.y));
    list.push_back(cv::Point2f(centerPoint.x + rx, centerPoint.y + dy));
    list.push_back(cv::Point2f(centerPoint.x + dx, centerPoint.y + ry));
    list.push_back(cv::Point2f(centerPoint.x, centerPoint.y + ry));
    list.push_back(cv::Point2f(centerPoint.x - dx, centerPoint.y + ry));
    list.push_back(cv::Point2f(centerPoint.x - rx, centerPoint.y + dy));
    list.push_back(cv::Point2f(centerPoint.x - rx, centerPoint.y));
    list.push_back(cv::Point2f(centerPoint.x - rx, centerPoint.y - dy));
    list.push_back(cv::Point2f(centerPoint.x - dx, centerPoint.y - ry));
    list.push_back(cv::Point2f(centerPoint.x, centerPoint.y - ry));
    list.push_back(cv::Point2f(centerPoint.x + dx, centerPoint.y - ry));
    list.push_back(cv::Point2f(centerPoint.x + rx, centerPoint.y - dy));
    list.push_back(cv::Point2f(centerPoint.x + rx, centerPoint.y));

    return list;
}

/*
 * 将一个圆角矩形转换为4段三次贝塞尔曲线这4段贝塞尔曲线按逆时针方向从第一象限到第四象限，每段4个点，
 * 第一段的点序号为[0,1,2,3]，其余段递增类推。
 * param:
    in: rect 矩形外框，规范化矩形
    in: rx X方向的圆角半径，非负数
    in: ry Y方向的圆角半径，非负数
 * return:
    out: 贝塞尔曲线的控制点，每段4个点，共16个点
*/
std::vector<cv::Point2f> BezierCurve::roundRectToBezier(cv::Rect2f rect, float rx, float ry)
{
    std::vector<cv::Point2f> list;
    cv::Point2f points[16];
    if (2 * rx > rect.width)
        rx = rect.width / 2.0f;
    if (2 * ry > rect.height)
        ry = rect.height / 2.0f;

    float dx = rect.width / 2.0f - rx;
    float dy = rect.height / 2.0f - ry;

    list = ellipseToBezier((rect.tl() + rect.br())*0.5f, rx, ry);

    for (int i = 3; i >= 1; i--)
    {
        for (int j = 3; j >= 0; j--)
            points[4 * i + j] = points[3 * i + j];
    }
    for (int i = 0; i < 4; i++)
    {
        float dx1 = (0 == i || 3 == i) ? dx : -dx;
        float dy1 = (0 == i || 1 == i) ? dy : -dy;
        for (int j = 0; j < 4; j++)
        {
            points[0].x = points[0].x + dx1;
            points[0].y = points[0].y + dy1;
        }
    }
    for(int loop=0; loop<16; loop++)
    {
        list[loop] = points[loop];
    }
    return list;
}

//椭圆弧段用贝塞尔曲线去代替
void BezierCurve::arcToBezier(cv::Point2f points[4],cv::Point2f centerPoint, float rx, float ry, double start_angle, double sweep_angle)
{
    double x0 = cos(sweep_angle / 2.0);
    double y0 = sin(sweep_angle / 2.0);
    double tx = (1.0 - x0) * 4.0 / 3.0;
    double ty = y0 - tx * x0 / y0;
    double px[4];
    double py[4];
    double curve[8];

    px[0] =  x0;
    py[0] = -y0;
    px[1] =  x0 + tx;
    py[1] = -ty;
    px[2] =  x0 + tx;
    py[2] =  ty;
    px[3] =  x0;
    py[3] =  y0;

    double sn = sin(start_angle + sweep_angle / 2.0);
    double cs = cos(start_angle + sweep_angle / 2.0);

    for(int loop = 0; loop < 4; loop++)
    {
         curve[loop * 2]     = centerPoint.x + rx * (px[loop] * cs - py[loop] * sn);
         curve[loop * 2 + 1] = centerPoint.y + ry * (px[loop] * sn + py[loop] * cs);
         points[loop].x = static_cast<float>(static_cast<float>(cvRound(curve[loop * 2])));
         points[loop].y = static_cast<float>(static_cast<float>(cvRound(curve[loop * 2 + 1])));
    }
}

//将一个椭圆弧转换为多段三次贝塞尔曲线
void BezierCurve::angleArcToBezier(cv::Point2f points[4], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle)
{
    // Compute bezier curve for arc centered along y axis
    // Anticlockwise: (0,-B), (x,-y), (x,y), (0,B)
    double sy = ry / rx;
    ry = rx;
    double B = ry * sin(sweepAngle / 2.0);
    double C = rx * cos(sweepAngle / 2.0);
    double A = rx - C;
    double X = A * 4.0 / 3.0;
    double Y = B - X * (rx - A)/B;
    points[0].x = static_cast<float>(static_cast<float>(cvRound(C)));
    points[0].y = static_cast<float>(static_cast<float>(cvRound(-B)));
    points[1].x = static_cast<float>(static_cast<float>(cvRound(C + X)));
    points[1].y = static_cast<float>(static_cast<float>(cvRound(-Y)));
    points[2].x = static_cast<float>(static_cast<float>(cvRound(C + X)));
    points[2].y = static_cast<float>(static_cast<float>(cvRound(Y)));
    points[3].x = static_cast<float>(static_cast<float>(cvRound(C)));
    points[3].y = static_cast<float>(static_cast<float>(cvRound(B)));
    // rotate to the original angle
    A = startAngle + sweepAngle / 2.0;
    double s = sin(A);
    double c = cos(A);
    for (int i = 0; i < 4; i++)
    {
        points[i].x = static_cast<float>(static_cast<float>(cvRound(centerPoint.x + points[i].x * c - points[i].y * s)));
        points[i].y = static_cast<float>(static_cast<float>(cvRound(centerPoint.y + points[i].x * s * sy + points[i].y * c * sy)));
    }
}

int BezierCurve::angleArcToBezierPlusSweep(cv::Point2f points[16], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle)
{
    double dx = rx * M;
    double dy = ry * M;
    int k, n;
    double endAngle;
    // 计算第一段椭圆弧的终止角度
    if (startAngle < MPI2)
    {
        // +Y
        endAngle = MPI2;
        k = 1;
    }
    else if (startAngle < MPI)
    {
        // -X
        endAngle = MPI;
        k = 2;
    }
    else if (startAngle < 3.0 * MPI2)
    {
        // -Y
        endAngle = 3.0*MPI2;
        k = 3;
    }
    else
    {
        // +X
        endAngle = M2PI;
        k = 0;
    }
    if (endAngle - startAngle > 1e-7) // 转换第一段椭圆弧
    {
        arcToBezier(points, centerPoint, rx, ry,startAngle, endAngle - startAngle);
        n = 4;
    }
    else
        n = 1; // 第一点在下边循环内设置

    sweepAngle -= (endAngle - startAngle);
    startAngle = endAngle;

    while (sweepAngle >= MPI2) // 增加整90度弧
    {
        if (k == 0) // 第一象限
        {
            points[n-1].x = static_cast<float>(cvRound(centerPoint.x + rx));
            points[n-1].y = static_cast<float>(cvRound(centerPoint.y + 0.0));
            points[n].x = static_cast<float>(cvRound(centerPoint.x + rx));
            points[n].y = static_cast<float>(cvRound(centerPoint.y + dy));
            points[n+1].x = static_cast<float>(cvRound(centerPoint.x + dx));
            points[n+1].y = static_cast<float>(cvRound(centerPoint.y + ry));
            points[n+2].x = static_cast<float>(cvRound(centerPoint.x + 0.0));
            points[n+2].y = static_cast<float>(cvRound(centerPoint.y + ry));
        }
        else if (k == 1) // 第二象限
        {
            points[n-1].x = static_cast<float>(cvRound(centerPoint.x + 0.0));
            points[n-1].y = static_cast<float>(cvRound(centerPoint.y + ry));
            points[n].x = static_cast<float>(cvRound(centerPoint.x - dx));
            points[n].y = static_cast<float>(cvRound(centerPoint.y + ry));
            points[n+1].x = static_cast<float>(cvRound(centerPoint.x - rx));
            points[n+1].y = static_cast<float>(cvRound(centerPoint.y + dy));
            points[n+2].x = static_cast<float>(cvRound(centerPoint.x - rx));
            points[n+2].y = static_cast<float>(cvRound(centerPoint.y + 0.0));
        }
        else if (k == 2) // 第三象限
        {
            points[n-1].x = static_cast<float>(cvRound(centerPoint.x - rx));
            points[n-1].y = static_cast<float>(cvRound(centerPoint.y + 0.0));
            points[n].x = static_cast<float>(cvRound(centerPoint.x - rx));
            points[n].y = static_cast<float>(cvRound(centerPoint.y - dy));
            points[n+1].x = static_cast<float>(cvRound(centerPoint.x - dx));
            points[n+1].y = static_cast<float>(cvRound(centerPoint.y - ry));
            points[n+2].x = static_cast<float>(cvRound(centerPoint.x + 0.0));
            points[n+2].y = static_cast<float>(cvRound(centerPoint.y - ry));
        }
        else // 第四象限
        {
            points[n-1].x = static_cast<float>(cvRound(centerPoint.x + 0.0));
            points[n-1].y = static_cast<float>(cvRound(centerPoint.y - ry));
            points[n].x = static_cast<float>(cvRound(centerPoint.x + dx));
            points[n].y = static_cast<float>(cvRound(centerPoint.y - ry));
            points[n+1].x = static_cast<float>(cvRound(centerPoint.x + rx));
            points[n+1].y = static_cast<float>(cvRound(centerPoint.y - dy));
            points[n+2].x = static_cast<float>(cvRound(centerPoint.x + rx));
            points[n+2].y = static_cast<float>(cvRound(centerPoint.y + 0.0));
        }
        k = (k + 1) % 4;
        n += 3;
        sweepAngle -= MPI2;
        startAngle += MPI2;
    }

    if (sweepAngle > 1e-7) // 增加余下的弧
    {
        arcToBezier(&points[n-1], centerPoint, rx, ry, startAngle, sweepAngle);
        n += 3;
    }
    return n;
}

/*将一个椭圆弧转换为多段三次贝塞尔曲线4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，每一段4个点，
 * 第一个点和最后一个点重合于+X轴上点(rx, 0)。
 * param:
    in: center 椭圆心
    in: rx 半长轴的长度
    in: ry 半短轴的长度，为0则取为rx
    in: startAngle 起始角度，弧度，相对于+X轴，逆时针为正
    in: sweepAngle 转角，弧度，相对于起始角度，逆时针为正
 * return:
    计算后的控制点数，点数小于4则给定参数有错误
    out: points 贝塞尔曲线的控制点，16个点
*/
int BezierCurve::tAngleArcToBezier(cv::Point2f points[16], cv::Point2f centerPoint, float rx, float ry, double startAngle, double sweepAngle)
{
     int n = 0;
     cv::Point2f tempPoint;

    if (fabs(rx) < 1e-7f || fabs(sweepAngle) < 1e-7f)
        return 0;
    if (fabs(ry) < 1e-7f)
        ry = rx;
    if (sweepAngle > M2PI)
        sweepAngle = M2PI;
    else if (sweepAngle < -M2PI)
        sweepAngle = -M2PI;

    if (fabs(sweepAngle) < MPI2 + 1e-7)
    {
        arcToBezier(points, centerPoint, rx, ry, startAngle, sweepAngle);
        n = 4;
    }
    else if (sweepAngle > 0)
    {
        startAngle = toRange(startAngle, 0.0, M2PI);
        n = angleArcToBezierPlusSweep(points, centerPoint, rx, ry, startAngle, sweepAngle);
    }
    else // sweepAngle < 0
    {
        double endAngle = startAngle + sweepAngle;
        sweepAngle = -sweepAngle;
        startAngle = toRange(endAngle, 0.0, M2PI);
        n = angleArcToBezierPlusSweep(points, centerPoint, rx, ry, startAngle, sweepAngle);
        for (int i = 0; i < n / 2; i++)
        {
            tempPoint = points[i];
            points[i] = points[n - 1 - i];
            points[n - 1 - i] = tempPoint;
        }
    }
    return n;
}

//去除连续的重复点
std::vector<cv::Point2f> BezierCurve::removeSamePoint(const std::vector<cv::Point2f>& points)
{
    int count = static_cast<int>(points.size());
    std::vector<cv::Point2f> resultPoints;
    int i = 0;
    int j = 0;
    while (i < count)
    {
        resultPoints.push_back(points[i]);
        for(j = i + 1; j<count; j++)
        {
            if (points[i] != points[j])
            {
                break;
            }
        }
        if (i == j-1)
        {
            i++;
        }
        else
        {
            i = j;
        }
    }
    return resultPoints;
}

//三次贝塞尔曲线算法
cv::Point2f BezierCurve::Bezier3(cv::Point2f startPoint, cv::Point2f ctrlPoint1, cv::Point2f ctrlPoint2, cv::Point2f endPoint, double t)
{
    cv::Point2f point;
    double a1 = pow((1 - t) * 1.0, 3);
    double a2 = pow((1 - t) * 1.0, 2) * 3.0 * t;
    double a3 = 3.0 * t * t * (1 - t);
    double a4 = t * t * t;

    double x = a1 * startPoint.x + a2 * ctrlPoint1.x + a3 * ctrlPoint2.x + a4 * endPoint.x;
    double y = a1 * startPoint.y + a2 * ctrlPoint1.y + a3 * ctrlPoint2.y + a4 * endPoint.y;

    point.x = static_cast<float>(x);
    point.y = static_cast<float>(y);
    return point;
}

//二次贝塞尔曲线算法
cv::Point2f BezierCurve::Bezier2(cv::Point2f startPoint, cv::Point2f ctrlPoint, cv::Point2f endPoint, double t)
{
    // 计算曲线点坐标，此为2次算法，改变此处可以实现多次曲线
    cv::Point2f point;
    double x = startPoint.x * pow(1 - t, 2)   +
                ctrlPoint.x * t * (1 - t) * 2 +
                  endPoint.x * pow(t, 2);
    double y = startPoint.y * pow(1 - t, 2)   +
                 ctrlPoint.y * t * (1 - t) * 2 +
                  endPoint.y * pow(t, 2);
    point.x = static_cast<float>(x);
    point.y = static_cast<float>(y);
    return point;
}

//一次贝塞尔曲线算法
cv::Point2f BezierCurve::Bezier1(cv::Point2f startPoint, cv::Point2f endPoint, double t)
{
    cv::Point2f point;
    double x = (1.0 - t) * 1.0 * startPoint.x + t * 1.0 * endPoint.x;
    double y = (1.0 - t) * 1.0 * startPoint.y + t * 1.0 * endPoint.y;
    point.x = static_cast<float>(x);
    point.y = static_cast<float>(y);
    return point;
}

//n次贝塞尔曲线算法
cv::Point2f BezierCurve::Beziern(const std::vector<cv::Point2f>& ctrlPoints, int n, double t)
{
    CV_Assert(n >= 2);
    std::vector<cv::Point2d> points;
    points.resize(n);
    for(int i = 1; i < n; ++i)
    {
         for(int j = 0; j < n - i; ++j)
         {
             if (i == 1) //i==1时,第一次迭代,由已知控制点计算
             {
                 points[j].x = ctrlPoints[j].x * (1 - t) + ctrlPoints[j + 1].x * t;
                 points[j].y = ctrlPoints[j].y * (1 - t) + ctrlPoints[j + 1].y * t;
                 continue;
             }
             //i != 1时,通过上一次迭代的结果计算
             points[j].x = points[j].x * (1 - t) + points[j + 1].x * t;
             points[j].y = points[j].y * (1 - t) + points[j + 1].y * t;
         }
    }
    return points[0];
}

//将数转换到数值范围[tmin, tmax)内
double BezierCurve::toRange(double value, double tmin, double tmax)
{
    while (value < tmin)
    {
        value += tmax - tmin;
    }
    while (value >= tmax)
    {
        value -= tmax - tmin;
    }
    return value;
}
