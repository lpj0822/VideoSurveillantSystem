#include "imageprocess.h"
#include <ctime>
#include <iostream>

ImageProcess::ImageProcess()
{
    init();
    std::cout << "ImageProcess()" << std::endl;
}

ImageProcess::~ImageProcess()
{
    std::cout << "~ImageProcess()" << std::endl;
}

//在图像上绘制中心点
void ImageProcess::drawCenter(cv::Mat &inFrame, const std::vector<cv::Point2f>& vectorPoint, cv::Scalar color)
{
    if (inFrame.empty())
    {
        return;
    }
    size_t countCenter=vectorPoint.size();
    for (size_t loop1=0; loop1<countCenter; loop1++)
    {
        cv::circle(inFrame, vectorPoint[loop1], 2, color, 2, 8);
    }
}

//在图像上绘制矩形框
void ImageProcess::drawRect(cv::Mat &inFrame, const std::vector<cv::Rect>& vectorRect, cv::Scalar color)
{
    //cv::Scalar color;
    if (inFrame.empty())
    {
        return;
    }
    size_t countRect = vectorRect.size();
    for (size_t loop1 = 0; loop1< countRect; loop1++)
    {
        //color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::rectangle(inFrame, vectorRect[loop1].tl(), vectorRect[loop1].br(), color, 2, 8, 0);
    }
}

//在图像上绘制多边形
void ImageProcess::drawPolygon(cv::Mat &inFrame, const std::vector<cv::Point>& polygon, cv::Scalar color)
{
    size_t index = 0;
    size_t num = polygon.size();
    for(int loop=0; loop<num; loop++)
    {
        index = (loop + 1) % num;
        cv::line(inFrame, polygon[loop], polygon[index], color, 2, 8);
    }
}

//绘制图像的轮廓
cv::Mat ImageProcess::drawObjectContours(const cv::Mat &inMat, const std::vector< std::vector<cv::Point> > &objectContours,
                                         const std::vector<cv::Rect> &objectRect, cv::Scalar color)
{
    //绘制出轮廓
    if (inMat.empty())
    {
        return cv::Mat(320, 480, CV_8UC3);
    }
    cv::Mat drawing = cv::Mat::zeros(inMat.size(), CV_8UC3);
    for (int loop1 = 0; loop1< objectContours.size(); loop1++)
    {
        //color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::drawContours(drawing, objectContours, loop1, color, 1, 8);
        cv::rectangle(drawing, objectRect[loop1].tl(), objectRect[loop1].br(), color, 1, 8, 0);
    }
    return drawing;
}

void ImageProcess::init()//初始化数据
{
    long seed = (long)time(0);
    rng = cv::RNG(seed);
}

