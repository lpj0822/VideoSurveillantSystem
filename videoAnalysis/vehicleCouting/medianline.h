#ifndef MEDIANLINE_H
#define MEDIANLINE_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QLine>

class MedianLine
{
public:
    MedianLine();
    ~MedianLine();

     QLine getMedianLine(const std::vector<cv::Point>& points,const int& direction);//根据多边形，得到大概的中位线
     void getMedianLine(const std::vector<cv::Point>& points,const int& direction,cv::Point& point1,cv::Point& point2);//根据多边形，得到大概的中位线

     int getMedianPosition(const std::vector<cv::Point>& points,const int& direction);//根据方向得到比较的中间位置

private:

};

#endif // MEDIANLINE_H
