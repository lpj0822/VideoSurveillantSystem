#include "geodesicsaliency.h"
#include <opencv2/imgproc.hpp>
#include <iostream>

GeodesicSaliency::GeodesicSaliency()
{
    std::cout << "GeodesicSaliency()" << std::endl;
}

GeodesicSaliency::~GeodesicSaliency()
{
    std::cout << "~GeodesicSaliency()" << std::endl;
}

cv::Mat GeodesicSaliency::getSaliency(const cv::Mat &src)
{
    cv::Mat rgbImg;
    cv::cvtColor(src, rgbImg, cv::COLOR_BGR2RGB);

    cv::Mat saliencyMap = cv::Mat::zeros(rgbImg.size(), CV_32FC1);
    int imgWidth = rgbImg.cols;
    int imgHeight = rgbImg.rows;
    int stride = (int)rgbImg.step;

    //initialize BGMap extractor
    //note that if we have multiple images to process, only need to initialize it once,
    //but the iMaxWidth and iMaxHeight should be set to the maximum size of all images
    //for the grid size (iPatchWidth and iPatchHeight), usually if the image size is about 400*400, we can set the grid size to 10*10
    int iMaxWidth = imgWidth;
    int iMaxHeight = imgHeight;
    int iPatchWidth = 10;
    int iPatchHeight = 10;

    //allocate memory for saliency map
    float *pfBGMap = new float[imgWidth * imgHeight];
    if(pfBGMap == NULL)
    {
        printf("memory error.\n");
        return saliencyMap;
    }

    if(!bgExtractor.Initialize(iMaxWidth, iMaxHeight, iPatchWidth, iPatchHeight))
    {
        printf("bgExtractor initialization error.\n");
        return saliencyMap;
    }

    //generate saliency map (saliency values have been normalized to 0-1)
    if(!bgExtractor.BGMapExtract((const unsigned char *)rgbImg.data, imgWidth, imgHeight, stride, pfBGMap))
    {
        printf("BG_map extraction error.\n");
        if(pfBGMap)
        {
            delete[] pfBGMap;
            pfBGMap = NULL;
        }
        return saliencyMap;
    }

    //save saliency map
    memcpy(saliencyMap.data, pfBGMap, sizeof(float) * imgWidth * imgHeight);

    //free spaces
    if(pfBGMap)
    {
        delete[] pfBGMap;
        pfBGMap = NULL;
    }

    return saliencyMap;
}
