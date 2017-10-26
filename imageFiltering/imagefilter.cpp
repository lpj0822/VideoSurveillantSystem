#include "imagefilter.h"
#include <iostream>

ImageFilter::ImageFilter()
{
    std::cout<<"ImageFilter()"<<std::endl;
}

ImageFilter::~ImageFilter()
{
    std::cout<<"~ImageFilter()"<<std::endl;
}

//图像滤波
void ImageFilter::filter(const cv::Mat &inMat,cv::Mat &outMat,int kernelSize,int type)
{
    if(inMat.empty())
    {
        std::cout<<"image empty!"<<std::endl;
        return;
    }
    switch(type)
    {
    case 1:
        cv::boxFilter(inMat, outMat, -1, cv::Size(kernelSize, kernelSize));//方框滤波
        break;
    case 2:
        cv::blur(inMat, outMat, cv::Size(kernelSize, kernelSize));//均值滤波
        break;
    case 3:
        cv::GaussianBlur(inMat, outMat, cv::Size(kernelSize, kernelSize), 3.5, 3.5); //高斯滤波
        break;
    case 4:
        cv::medianBlur(inMat,outMat,kernelSize);//中值滤波
        break;
    case 5:
        cv::bilateralFilter(inMat,outMat,kernelSize,kernelSize*2,kernelSize/2);//双边滤波,速度慢
        break;
    }
}

//导向滤波器
void ImageFilter::guidedFilter(const cv::Mat &inMat ,cv::Mat &outMat,bool enhanced,int radius, double eps, int depth)
{
    CV_Assert(inMat.channels()==3||outMat.channels()==1);
    cv::Mat p;
    if(inMat.empty())
    {
        std::cout<<"image empty!"<<std::endl;
        return;
    }
    if(enhanced)
    {
        if(inMat.channels()==3)
        {
            inMat.convertTo(p,CV_32FC3,1.0/255.0);
        }
        else
        {
            inMat.convertTo(p,CV_32FC1,1.0/255.0);
        }
        cv::Mat result=GuidedFilter(p, radius, eps).filter(p, depth);
        outMat=(p - result) * 5 + result;
        outMat.convertTo(outMat,inMat.type(),255.0);
    }
    else
    {
        eps *= 255 * 255;   // Because the intensity range of our images is [0, 255]
        outMat=GuidedFilter(inMat, radius, eps).filter(inMat,depth);
    }
}

//导向滤波器
void ImageFilter::guidedFilter(const cv::Mat &inMat,cv::Mat &outMat, int radius, double eps, int depth)
{
    CV_Assert(inMat.channels()==3||outMat.channels()==1);
    cv::Mat I;
    inMat.convertTo(I,CV_32F,1.0/255.0);
    cv::ximgproc::guidedFilter(I,inMat,outMat,radius,eps,depth);
}

void ImageFilter::guidedFilter(const cv::Mat& inMat,cv::Mat &I,cv::Mat &outMat,int radius, double eps, int depth)
{
    cv::Mat tempI;
    if(inMat.empty()||I.empty())
    {
        std::cout<<"image empty!"<<std::endl;
        return;
    }
    //eps *= 255 * 255;   // Because the intensity range of our images is [0, 255]
    //outMat=GuidedFilter(I, radius, eps).filter(inMat, depth);
    I.convertTo(tempI,CV_32F,1.0/255.0);
    cv::ximgproc::guidedFilter(tempI,inMat,outMat,radius,eps,depth);
}
