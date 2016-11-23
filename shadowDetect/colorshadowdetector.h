/**
 * Implemented from:
 *    Detecting moving objects, ghosts, and shadows in video streams
 *    Cucchiara et al. (PAMI 2003)
 *
 * Extended to use observation windows as proposed in:
 *    Cast shadow segmentation using invariant color features
 *    Salvador et al. (CVIU 2004)
 */
#ifndef COLORSHADOWDETECTOR_H
#define COLORSHADOWDETECTOR_H

#include "utility/ConnCompGroup.h"

class ColorShadowDetector
{
public:
    ColorShadowDetector();
    ~ColorShadowDetector();

    void removeShadows(const cv::Mat& frame, const cv::Mat& fg, const cv::Mat& bg, cv::Mat& srMask);//检测阴影
    void initData();//初始化参数

private:
    void extractDarkPixels(const cv::Mat& hsvFrame, const ConnCompGroup& fg, const cv::Mat& hsvBg,ConnCompGroup& darkPixels);
    void extractShadows(const cv::Mat& hsvFrame, const ConnCompGroup& darkPixels, const cv::Mat& hsvBg,ConnCompGroup& shadows);

private:

    bool firstTime;

    //参数
    int winSize;
    bool cleanSrMask;
    int hThresh;
    int sThresh;
    float vThreshUpper;
    float vThreshLower;

    void init();

private:
    void saveConfig();
    void loadConfig();
};

#endif // COLORSHADOWDETECTOR_H
