#include "meanshiftsegmentation.h"
#include <iostream>

static const int spatialRad = 25;
static const int colorRad = 45;
static const int maxPryLevel = 1;

MeanShiftSegmentation::MeanShiftSegmentation()
{
    std::cout << "MeanShiftSegmentation()" << std::endl;
}

MeanShiftSegmentation::~MeanShiftSegmentation()
{
    std::cout << "~MeanShiftSegmentation()" << std::endl;
}

cv::Mat MeanShiftSegmentation::getSegmentMap(const cv::Mat &src, const cv::Mat &weightMap)
{
    cv::Mat imageSegmentMap;
    if(src.empty())
    {
        return imageSegmentMap;
    }
    cv::pyrMeanShiftFiltering(src, imageSegmentMap, spatialRad, colorRad, maxPryLevel);
    return imageSegmentMap;
}
