#include "geometrycalculations.h"
#include <iostream>

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
      if (box1.x > box2.x+box2.width) { return 0.0f; }
      if (box1.y > box2.y+box2.height) { return 0.0f; }
      if (box1.x+box1.width < box2.x) { return 0.0f; }
      if (box1.y+box1.height < box2.y) { return 0.0f; }
      float colInt =  std::min(box1.x+box1.width,box2.x+box2.width) - std::max(box1.x, box2.x);
      float rowInt =  std::min(box1.y+box1.height,box2.y+box2.height) - std::max(box1.y,box2.y);
      float intersection = colInt * rowInt;
      float area1 = box1.width*box1.height;
      float area2 = box2.width*box2.height;
      if(((intersection>area1-0.1)&&(intersection<area1+0.1))||((intersection>area2-0.1)&&(intersection<area2+0.1)))//包含
      {
          return 1.0f;
      }
      return intersection / (area1 + area2 - intersection*2);
}

//计算两个多边形的相交度
float GeometryCalculations::polygonOverlap(const std::vector<cv::Point>& polygon0,const std::vector<cv::Point>& polygon1)
{
    std::vector<cv::Point> crossPolygon=intersectPolygon(polygon0,polygon1);
    if(crossPolygon.size()<=2)
    {
        return 0.0f;
    }
    float crossArea=(float)cv::contourArea(crossPolygon);
    float area0=(float)cv::contourArea(polygon0);
    float area1=(float)cv::contourArea(polygon1);
    if(((crossArea>area0-0.1)&&(crossArea<area0+0.1))||((crossArea>area1-0.1)&&(crossArea<area1+0.1)))//包含
    {
        return 1.0f;
    }
    return crossArea / (area0 + area1 - crossArea*2);
}

//将矩形转换为点存储
std::vector<cv::Point> GeometryCalculations::covRectPolygon(cv::Rect rect)
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
    int number0=(int)polygon0.size();
    int number1=(int)polygon1.size();
    cv::Point crossPoint;
    cv::Point p0,p1,p2,p3;
    corssPolygon.clear();
    for (int i = 0 ; i < number0 ;i++)
    {
        if(cv::pointPolygonTest(polygon1,polygon0[i],false)>0)
        {
            corssPolygon.push_back(polygon0[i]);
        }
    }

    for (int i = 0 ; i < number1 ;i++)
    {
        if(cv::pointPolygonTest(polygon0,polygon1[i],false)>0)
        {
            corssPolygon.push_back(polygon1[i]);
        }
    }

    for (int i = 0 ; i < number0 ;i++)
    {
        p0 = polygon0[i];
        p1 = polygon0[(i+1)%number0];
        for (int j = 0 ; j < number1 ;j++)
        {
            p2 = polygon1[j];
            p3 = polygon1[(j+1)%number1];
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
bool GeometryCalculations::segementIntersection(cv::Point startP0,cv::Point endP0,cv::Point startP1,cv::Point endP1,cv::Point& crossPoint)
{
    cv::Point2f tempPoint;
    bool ok = segementIntersection(cv::Point2f(startP0),cv::Point2f(endP0),cv::Point2f(startP1),cv::Point2f(endP1),tempPoint);
    if( ok )
        crossPoint = cv::Point((int)tempPoint.x,(int)tempPoint.y);
    return ok;
}

//求两条线段的交点
bool GeometryCalculations::segementIntersection(cv::Point2f startP0,cv::Point2f endP0,cv::Point2f startP1,cv::Point2f endP1,cv::Point2f& crossPoint)
{
//    cv::Point2f vector1, vector2;
//    vector1 = cv::Point2f(endP0.x - startP0.x, endP0.y - startP0.y);
//    vector2 = cv::Point2f(endP1.x - startP1.x, endP1.y - startP1.y);

    float vector0_x = endP0.x - startP0.x;
    float vector0_y = endP0.y - startP0.y;
    float vector1_x = endP1.x - startP1.x;
    float vector1_y = endP1.y - startP1.y;
    float denom = vector0_x * vector1_y - vector1_x * vector0_y;//向量叉乘

    if(denom==0)//平行
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

//点是否在多边形内
bool GeometryCalculations::pointInPolygon(cv::Point point,const std::vector<cv::Point>& polygon)
{
    int i=0, j=0;
    bool isIn = false;
    int number=(int)polygon.size();
    if(number<=2)
        return false;
    for(i = 0, j = number - 1; i < number; j = i++)
    {
        if( ( (polygon[i].y >= point.y ) != (polygon[j].y >= point.y) ) &&
                (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
            isIn = !isIn;
    }
    return isIn;
}

//点是否在多边形内
bool GeometryCalculations::pointInPolygon(cv::Point2f point,const std::vector<cv::Point2f>& polygon)
{

    if(cv::pointPolygonTest(polygon,point,false)>=0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//点是否在多边形内
bool GeometryCalculations::pointInPolygon(cv::Point2f point,const std::vector<cv::Point>& polygon)
{
    if(cv::pointPolygonTest(polygon,point,false)>=0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//点是否在多边形内
bool GeometryCalculations::pointInPolygon(cv::Point point,const std::vector<cv::Point2f>& polygon)
{
    if(cv::pointPolygonTest(polygon,point,false)>=0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//点是否在多边形内
bool pointInPolygon(cv::Point2f point,const std::vector<cv::Point2f>& polygon)
{
    bool isIn = false;
    int number=(int)polygon.size();
    int i=0, j=number-1;
    if(number<=2)
        return false;

    for (i=0;i<number; i++)
    {
        if((polygon[i].y< point.y && polygon[j].y>=point.y||polygon[j].y<point.y && polygon[i].y>=point.y)
                &&(polygon[i].x<=point.x || polygon[j].x<=point.x))
        {
          if(polygon[i].x+(point.y-polygon[i].y)/(polygon[j].y-polygon[i].y)*(polygon[j].x-polygon[i].x)<point.x)
          {
            isIn=!isIn;
          }
        }
        j=i;
    }
    return isIn;
}

//得到多边形的中心
cv::Point GeometryCalculations::getPolygonCenter(const std::vector<cv::Point>& polygon)
{
    cv::Point center;
    int number=(int)polygon.size();
    center.x = 0;
    center.y = 0;
    for (int i = 0 ; i < number ; i++ )
    {
        center.x += polygon[i].x;
        center.y += polygon[i].y;
    }
    center.x /= number;
    center.y /= number;
    return center;
}

//根据点的角度对多边形的点排序
void GeometryCalculations::pointsOrdered(std::vector<cv::Point>& polygon)
{
    int number=(int)polygon.size();
    if( number <= 2)
        return;
    cv::Point center = getPolygonCenter(polygon);
    //vector<PointAngle> tempPolygon;
    //PointAngle angleP;
    PointAngle *tempPolygon=NULL;
    tempPolygon=new PointAngle[number];
    if(tempPolygon==NULL)
    {
        return;
    }
    for (int i = 0 ; i < number ; i++ )
    {
        tempPolygon[i].point.x = polygon[i].x;
        tempPolygon[i].point.y = polygon[i].y;
        tempPolygon[i].angle = atan2f((float)(polygon[i].y - center.y),(float)(polygon[i].x - center.x));
        //angleP.point=polygon[i];
        //angleP.angle=atan2f((float)(polygon[i].y - center.y),(float)(polygon[i].x - center.x));
        //tempPolygon.push_back(angleP);
    }
    qsort(tempPolygon,number,sizeof(PointAngle),comp_point_with_angle);
    for (int i = 0 ; i < number ; i++ )
    {
        polygon[i].x = tempPolygon[i].point.x;
        polygon[i].y = tempPolygon[i].point.y;
    }
    if(tempPolygon)
    {
        delete tempPolygon;
        tempPolygon=NULL;
    }
}

//计算多边形的面积
float GeometryCalculations::polygonArea(const std::vector<cv::Point>& polygon)
{
    float tempArea = 0.f;
    int number=(int)polygon.size();
    for (int i = 0 ; i < number ; i++ )
    {
        int j = (i+1)%number;
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

