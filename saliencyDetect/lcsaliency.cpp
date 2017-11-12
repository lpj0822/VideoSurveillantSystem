#include "lcsaliency.h"
#include <iostream>

LCSaliency::LCSaliency()
{
    std::cout<<"LCSaliency()"<<std::endl;
}

LCSaliency::~LCSaliency()
{
    std::cout<<"~LCSaliency()"<<std::endl;
}

cv::Mat LCSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3||src.channels() == 1);

    cv::Mat img;
    if(src.channels() == 3)
    {
        img = rtcpRgb2gray.BGR2Gray(src);
        //cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img = src.clone();
    }
    double f[256], s[256];
    memset(f, 0, 256*sizeof(double));
    memset(s, 0, 256*sizeof(double));
    for (int r = 0; r < img.rows; r++)
    {
        uchar* data = img.ptr<uchar>(r);
        for (int c = 0; c < img.cols; c++)
        {
            int index=data[c];
            f[index] += 1;
        }
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            s[i] += abs(i - j) * f[j];
        }
    }
    cv::Mat saliencyMap = cv::Mat(src.size(), CV_32FC1);
    for (int r = 0; r < img.rows; r++)
    {
        uchar* data = img.ptr<uchar>(r);
        float* sal = saliencyMap.ptr<float>(r);
        for (int c = 0; c < img.cols; c++)
        {
            int index=data[c];
            sal[c] = (float)s[index];
        }
    }
    cv::normalize(saliencyMap, saliencyMap,1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}
