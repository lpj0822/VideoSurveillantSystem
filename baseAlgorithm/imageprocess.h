#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class ImageProcess
{
public:
    ImageProcess();
    ~ImageProcess();

    void drawCenter(cv::Mat &inFrame, const std::vector<cv::Point2f>& vectorPoint, cv::Scalar color=cv::Scalar(255,255,255));//在图像上绘制中心点
    void drawRect(cv::Mat &inFrame, const std::vector<cv::Rect>& vectorRect, cv::Scalar color=cv::Scalar(255,255,255));//在图像上绘制矩形框
    void drawPolygon(cv::Mat &inFrame, const std::vector<cv::Point>& polygon, cv::Scalar color=cv::Scalar(0,0,255));//在图像上绘制多边形
    cv::Mat drawObjectContours(const cv::Mat &inMat, const std::vector< std::vector<cv::Point> > &objectContours,
                               const std::vector<cv::Rect> &objectRect, cv::Scalar color=cv::Scalar(255,255,255));//绘制图像的轮廓

private:
    cv::RNG rng;//随机数生成器
    void init();
};

#endif // IMAGEPROCESS_H
