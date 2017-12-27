#ifndef FRAMEFOREGROUND_H
#define FRAMEFOREGROUND_H

#include "algorithmBGS//MixtureOfGaussianV2BGS.h"
#include "algorithmBGS/PixelBasedAdaptiveSegmenter.h"
#include "algorithmBGS/ViBeBGS.h"
#include "shadowDetect/colorshadowdetector.h"
#include "imageFiltering/imagefilter.h"
#include <vector>

class FrameForeground
{
public:
    FrameForeground();
    ~FrameForeground();

    void initData();//初始化参数

    std::vector<cv::Rect> getFrameForegroundRect(const cv::Mat& inFrame,float minBox=250.0f);//处理视频帧得到前景目标
    std::vector<cv::Point2f> getFrameForegroundCenter(const cv::Mat& inFrame,float minBox=250.f);//处理视频帧得到前景目标的中心
    std::vector<cv::Point2f> getFrameForegroundCentroid(const cv::Mat& inFrame,float minBox=250.0f);//处理视频帧得到目标的质心

    //得到前景图像
    void getFrameForeground(const cv::Mat& inFrame, cv::Mat& foregroundFrame, cv::Mat& backgroundFrame=cv::Mat());
    //得到去除阴影的前景图像
    void getFrameRemoveShadowForeground(const cv::Mat& inFrame, cv::Mat& foregroundFrame);
    //得到目标的多边形区域前景图像
    void getFrameForeground(const cv::Mat& inFrame, cv::Mat &foregroundFrame, float minBox);

private:
    IBGS *bgs;//背景分割算法
    ColorShadowDetector *colorShadowDetector;//阴影检测器
    ImageFilter *imageFilter;//图像滤波

    bool isFirstRun;//第一次运行
    std::string bgsName;//背景分割算法名称
    std::vector< std::vector<cv::Point> > objectContours;//目标多边形轮廓
    cv::Mat elementBGS;//自定义核，用于背景检测形态学操作
    cv::Mat elementRemoveShadow;//自定义核，用于除去阴影后形态学操作

    int flags;//使用前景提取的算法
    bool enableMorphology;//是否进行形态学操作
    int elementSizeBGS;//背景检测形态学操作尺寸大小
    int elementSizeRemoveShadow;//除去阴影后形态学操作尺寸大小
    int cannyThreshold;//canny算子的低阈值
    bool enableImageFilter;//是否进行图像滤波
    int imageFilterType;//使用的滤波器
    int imageFilterSize;//滤波器尺寸
//    bool enableOutputBlur;//前景图像是否滤波
//    int outputFilterType;
//    int outputFilterSize;
    bool enableShowForeground;//是否显示前景

private:
    //计算目标的多边形轮廓
    void calculateFrameForegroundContours(const cv::Mat& inFrame);

    void init();
    void saveConfig();
    void loadConfig();
};

#endif // FRAMEFOREGROUND_H
