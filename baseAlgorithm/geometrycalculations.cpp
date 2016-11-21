#include "geometrycalculations.h"
#include <iostream>

struct PointAngle
{
    cv::Point point;
    float angle;
};//点的角度数据结构

int comp_point_with_angle(const void * a, const void * b);//排序函数

GeometryCalculations::GeometryCalculations()
{
    std::cout << "GeometryCalculations()" << std::endl;
}

GeometryCalculations::~GeometryCalculations()
{
    std::cout << "~GeometryCalculations()" << std::endl;
}


//计算两个矩形的相交度
float GeometryCalculations::rectOverlap(const cv::Rect& box1,const cv::Rect& box2)
{
    cv::Point bottomPoint1 = box1.br();
    cv::Point bottomPoint2 = box2.br();
    if (box1.x >= bottomPoint2.x || box1.y >= bottomPoint2.y || bottomPoint1.x <= box2.x || bottomPoint1.y <= box2.y)
    {
        return 0.0f;
    }
    float crossWidth =  static_cast<float>(std::min(bottomPoint1.x, bottomPoint2.x) - std::max(box1.x, box2.x));
    float crossHeight =  static_cast<float>(std::min(bottomPoint1.y, bottomPoint1.y) - std::max(box1.y, box2.y));
    float intersection = crossWidth * crossHeight;
    float area1 = static_cast<float>(box1.area());
    float area2 = static_cast<float>(box2.area());
    if (((intersection > area1-0.1f)&&(intersection < area1 + 0.1f))||((intersection> area2 - 0.1f)&&(intersection < area2 + 0.1f)))//包含
    {
          return 1.0f;
    }
    return intersection / (area1 + area2 - intersection * 2);
}

//计算两个多边形的相交度
float GeometryCalculations::polygonOverlap(const std::vector<cv::Point>& polygon0,const std::vector<cv::Point>& polygon1)
{
    std::vector<cv::Point> crossPolygon = intersectPolygon(polygon0,polygon1);
    if (crossPolygon.size() <= 2)
    {
        return 0.0f;
    }
    float crossArea = static_cast<float>(cv::contourArea(crossPolygon));
    float area0 = static_cast<float>(cv::contourArea(polygon0));
    float area1 = static_cast<float>(cv::contourArea(polygon1));
    if(((crossArea>area0-0.1)&&(crossArea<area0+0.1))||((crossArea>area1-0.1)&&(crossArea<area1+0.1)))//包含
    {
        return 1.0f;
    }
    return crossArea / (area0 + area1 - crossArea*2);
}

//将矩形转换为点存储
std::vector<cv::Point> GeometryCalculations::covRectPolygon(const cv::Rect& rect)
{
   std::vector<cv::Point> polygon;
   polygon.clear();
   polygon.push_back(rect.tl());
   polygon.push_back(cv::Point(rect.x+rect.width,rect.y));
   polygon.push_back(rect.br());
   polygon.push_back(cv::Point(rect.x,rect.y+rect.height));
   return polygon;
}

//计算相交的多边形
std::vector<cv::Point> GeometryCalculations::intersectPolygon(const std::vector<cv::Point>& polygon0,const std::vector<cv::Point>& polygon1)
{
    std::vector<cv::Point> corssPolygon;
    int count0 = static_cast<int>(polygon0.size());
    int count1 = static_cast<int>(polygon1.size());
    cv::Point crossPoint;
    cv::Point p0, p1, p2, p3;
    corssPolygon.clear();
    for (int i = 0 ; i < count0 ;i++)
    {
        if(cv::pointPolygonTest(polygon1, polygon0[i], false) > 0)
        {
            corssPolygon.push_back(polygon0[i]);
        }
    }

    for (int i = 0 ; i < count1 ;i++)
    {
        if(cv::pointPolygonTest(polygon0, polygon1[i], false) > 0)
        {
            corssPolygon.push_back(polygon1[i]);
        }
    }

    for (int i = 0 ; i < count0 ;i++)
    {
        p0 = polygon0[i];
        p1 = polygon0[(i + 1) % count0];
        for (int j = 0 ; j < count1 ; j++)
        {
            p2 = polygon1[j];
            p3 = polygon1[(j + 1) % 1];
            if(segementIntersection(p0,p1,p2,p3,crossPoint))
            {
                corssPolygon.push_back(crossPoint);
            }
        }
    }
    pointsOrdered(corssPolygon);
    return corssPolygon;
}

//求两条线段的交点
bool GeometryCalculations::segementIntersection(cv::Point startP0, cv::Point endP0, cv::Point startP1, cv::Point endP1, cv::Point& crossPoint)
{
    cv::Point2f tempPoint;
    bool ok = segementIntersection(cv::Point2f(startP0), cv::Point2f(endP0), cv::Point2f(startP1), cv::Point2f(endP1),tempPoint);
    if (ok)
        crossPoint = cv::Point(static_cast<int>(tempPoint.x), static_cast<int>(tempPoint.y));
    return ok;
}

//求两条线段的交点
bool GeometryCalculations::segementIntersection(cv::Point2f startP0, cv::Point2f endP0, cv::Point2f startP1, cv::Point2f endP1, cv::Point2f& crossPoint)
{
    float vector0_x = endP0.x - startP0.x;
    float vector0_y = endP0.y - startP0.y;
    float vector1_x = endP1.x - startP1.x;
    float vector1_y = endP1.y - startP1.y;
    float denom = vector0_x * vector1_y - vector1_x * vector0_y;//向量叉乘

    if (denom == 0)//平行
    {
        return false;
    }

    bool denom_positive = denom > 0;

    float vector01_x = startP0.x - startP1.x;
    float vector01_y = startP0.y - startP1.y;
    float s_numer = vector0_x * vector01_y - vector0_y * vector01_x;

    if((s_numer < 0.f) == denom_positive)
    {
        return false;
    }

    float t_numer = vector1_x * vector01_y - vector1_y * vector01_x;
    if((t_numer < 0) == denom_positive)
    {
        return false;
    }

    if((s_numer > denom) == denom_positive || (t_numer > denom) == denom_positive)
    {
        return false;
    }

    float t = t_numer / denom;

    crossPoint = cv::Point2f(startP0.x + (t * vector0_x), startP0.y + (t * vector0_y) );
    return true;
}

/*
 * 点是否在多边形内:
 * 将测试点的Y坐标与多边形的每一个点进行比较，我们会得到一个测试点所在的行与多边形边的交点的列表。
 * 如果测试点的两边点的个数都是奇数个则该测试点在多边形内，否则在多边形外。
 * 这个算法适用于顺时针和逆时针绘制的多边形。
 * 如果测试点刚好在多边形的边上，则这种算法得到的结果是不确定的。
 */
bool GeometryCalculations::pointInPolygon(cv::Point point, const std::vector<cv::Point>& polygon)
{
    bool isIn = false;
    std::vector<cv::Point2f> tempPolygon;
    int count = static_cast<int>(polygon.size());
    for (int i=0; i<count; i++)
        tempPolygon.push_back(cv::Point2f(polygon[i]));
    isIn = pointInPolygon(cv::Point2f(point), tempPolygon);
    return isIn;
}

bool GeometryCalculations::pointInPolygon(cv::Point2f point,const std::vector<cv::Point2f>& polygon)
{
    bool isIn = false;
    int count = static_cast<int>(polygon.size());
    int i=0, j=count-1;
    if (count<=2)
        return false;

    for (i=0; i<count; i++)
    {
        if((polygon[i].y < point.y && polygon[j].y >= point.y || polygon[j].y < point.y && polygon[i].y>=point.y)
                &&(polygon[i].x<=point.x || polygon[j].x<=point.x))
        {
          if(polygon[i].x+(point.y-polygon[i].y)/(polygon[j].y-polygon[i].y)*(polygon[j].x-polygon[i].x)<point.x)
          {
              isIn = !isIn;
          }
        }
        j = i;
    }
    return isIn;
}

//得到多边形的中心
cv::Point GeometryCalculations::getPolygonCenter(const std::vector<cv::Point>& polygon)
{
    cv::Point center;
    int count = static_cast<int>(polygon.size());
    center.x = 0;
    center.y = 0;
    for (int i = 0 ; i < count ; i++ )
    {
        center.x += polygon[i].x;
        center.y += polygon[i].y;
    }
    center.x /= count;
    center.y /= count;
    return center;
}

//根据点的角度对多边形的点排序
void GeometryCalculations::pointsOrdered(std::vector<cv::Point>& polygon)
{
    int count = static_cast<int>(polygon.size());
    if (count <= 2)
        return;
    cv::Point center = getPolygonCenter(polygon);
    PointAngle *tempPolygon = nullptr;
    tempPolygon = new PointAngle[count];
    if(tempPolygon == nullptr)
    {
        return;
    }
    for (int i = 0 ; i < count ; i++ )
    {
        tempPolygon[i].point.x = polygon[i].x;
        tempPolygon[i].point.y = polygon[i].y;
        tempPolygon[i].angle = atan2f((float)(polygon[i].y - center.y),(float)(polygon[i].x - center.x));
    }
    //快速排序
    qsort(tempPolygon, count, sizeof(PointAngle), comp_point_with_angle);
    for (int i = 0 ; i < count ; i++ )
    {
        polygon[i].x = tempPolygon[i].point.x;
        polygon[i].y = tempPolygon[i].point.y;
    }
    if (tempPolygon != nullptr)
    {
        delete[] tempPolygon;
        tempPolygon = nullptr;
    }
}

//计算多边形的面积
float GeometryCalculations::polygonArea(const std::vector<cv::Point>& polygon)
{
    float tempArea = 0.f;
    int count = static_cast<int>(polygon.size());
    for (int i = 0 ; i < count ; i++ )
    {
        int j = (i + 1) % count;
        tempArea += polygon[i].x * polygon[j].y;
        tempArea -= polygon[i].y * polygon[j].x;
    }
    return 0.5f * fabs(tempArea);
}

//排序函数
int comp_point_with_angle(const void * a, const void * b)
{
    if ( ((PointAngle*)a)->angle <  ((PointAngle*)b)->angle )
        return -1;
    else if ( ((PointAngle*)a)->angle > ((PointAngle*)b)->angle )
        return 1;
    else //if ( ((PointAngle*)a)->angle == ((PointAngle*)b)->angle ) return 0;
        return 0;
}

