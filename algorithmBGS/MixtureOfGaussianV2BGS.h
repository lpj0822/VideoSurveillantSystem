#ifndef MIXTUREOFGAUSSIANV2BGS_H
#define MIXTUREOFGAUSSIANV2BGS_H

#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"

class MixtureOfGaussianV2BGS : public IBGS
{

public:
    MixtureOfGaussianV2BGS();
    ~MixtureOfGaussianV2BGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
    void initMOG2(int history=500, double varThreshold=16);//初始化混合高斯模型

private:
    void init();

private:
    bool firstTime;
    cv::Ptr<cv::BackgroundSubtractorMOG2> mog2;
    cv::Mat img_foreground;
    double alpha;
    bool enableThreshold;
    int threshold;
    bool enableDetectShadow;
    double shadowThreshold;
    bool showOutput;

private:
    void saveConfig();
    void loadConfig();
};
#endif //MIXTUREOFGAUSSIANV2BGS_H
