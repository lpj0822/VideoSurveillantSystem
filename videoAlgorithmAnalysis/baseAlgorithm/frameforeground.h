#ifndef FRAMEFOREGROUND_H
#define FRAMEFOREGROUND_H

#include "algorithmBGS//MixtureOfGaussianV2BGS.h"
#include "algorithmBGS/PBAS/PixelBasedAdaptiveSegmenter.h"
#include "algorithmBGS/PBAS/vibebackgroundsegmenter.h"
#include "shadowDetect/colorshadowdetector.h"
#include "imagefilter.h"
#include <vector>

class FrameForeground
{
public:
    FrameForeground();
    ~FrameForeground();

    void initData();//初始化参数

    std::vector<cv::Rect> getFrameForegroundRect(cv::Mat inFrame,float minBox=250.0f);//处理视频帧得到前景目标
    std::vector<cv::Point2f> getFrameForegroundCenter(cv::Mat inFrame,float minBox=250.f);//处理视频帧得到前景目标的中心
    std::vector<cv::Point2f> getFrameForegroundCentroid(cv::Mat inFrame,float minBox=250.0f);//处理视频帧得到目标的质心

    void getFrameForeground(cv::Mat inFrame,cv::Mat& foregroundFrame);//得到前景图像
    void getFrameRemoveShadowForeground(cv::Mat inFrame,cv::Mat& foregroundFrame);//得到去除阴影的前景图像
    void getFrameForeground(cv::Mat inFrame, cv::Mat &foregroundFrame,float minBox);//得到目标的多边形区域前景图像

private:
    IBGS *bgs;//背景分割算法
    ColorShadowDetector *colorShadowDetector;//阴影检测器
    ImageFilter *imageFilter;//图像滤波

private:

    cv::Mat elementBGS;//自定义核，用于背景检测形态学操作
    cv::Mat elementRemove;//自定义核，用于除去阴影后形态学操作

    int flags;//使用前景提取的算法
    int elementSizeBGS;//背景检测形态学操作尺寸大小
    int elementSizeRemove;//除去阴影后形态学操作尺寸大小
    int cannyThreshold;//canny算子的低阈值
    bool enableImageFilter;//是否进行图像滤波
    int filterType;//使用的滤波器
    int filterSize;//滤波器尺寸
//    bool enableOutputBlur;//前景图像是否滤波
//    int outputFilterType;
//    int outputFilterSize;
    bool enableShowForeground;//是否显示前景

    bool isFirstRun;//第一次运行

    std::string bgsName;//背景分割算法名称

    void init();

private:
    void saveConfig();
    void loadConfig();
};

#endif // FRAMEFOREGROUND_H
