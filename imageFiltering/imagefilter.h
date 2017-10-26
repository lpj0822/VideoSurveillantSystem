#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ximgproc.hpp>
#include "guidedfilter.h"

class ImageFilter
{
public:
    ImageFilter();
    ~ImageFilter();

    void filter(const cv::Mat &inMat,cv::Mat &outMat,int kernelSize,int type);//图像滤波
    //导向滤波器
    void guidedFilter(const cv::Mat &inMat ,cv::Mat &outMat,bool enhanced,int radius, double eps, int depth = -1);
    void guidedFilter(const cv::Mat &inMat,cv::Mat &outMat, int radius, double eps, int depth=-1);
    void guidedFilter(const cv::Mat &inMat,cv::Mat &I ,cv::Mat &outMat, int radius, double eps, int depth=-1);
};

#endif // IMAGEFILTER_H
