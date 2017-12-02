#include "ftsaliency.h"
#include <iostream>

FTSaliency::FTSaliency()
{
    std::cout << "FTSaliency()" << std::endl;
}

FTSaliency::~FTSaliency()
{
    std::cout << "~FTSaliency()" << std::endl;
}

cv::Mat FTSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels()==3);

    cv::Mat input;
    cv::Mat saliencyMap(src.size(), CV_32FC1);
    cv::Mat tempLab;
    src.convertTo(input,CV_32FC3, 1.0/255);
    cv::cvtColor(input, tempLab, cv::COLOR_BGR2Lab);
    cv::GaussianBlur(tempLab, tempLab, cv::Size(5, 5), 0,0);
    cv::Scalar colorMean = cv::mean(tempLab);
    for (int r = 0; r < tempLab.rows; r++)
    {
        float *dataS = saliencyMap.ptr<float>(r);
        float *lab = tempLab.ptr<float>(r);
        for (int c = 0; c < tempLab.cols; c++, lab += 3)
        {
            dataS[c] = (float)((colorMean[0] - lab[0])*(colorMean[0] - lab[0]) +
                    (colorMean[1] - lab[1])*(colorMean[1] - lab[1]) +
                    (colorMean[2] - lab[2])*(colorMean[2] - lab[2]));
        }
    }
    cv::normalize(saliencyMap, saliencyMap, 1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}
