#ifndef OBJECTRECOGNITION_H
#define OBJECTRECOGNITION_H

#include "algorithmClassify/mycascadeclassifier.h"
#include "frameforeground.h"
#include "imageprocess.h"

class ObjectRecognition
{
public:
    ObjectRecognition();
    ~ObjectRecognition();

    void initData(std::string classifierName);

    std::vector<cv::Rect> getFrameCarObejctdRect(cv::Mat inFrame, int minSize,float minBox=250.0f);//处理视频帧得到前景目标
    std::vector<cv::Point2f> getFrameCarObejctdCenter(cv::Mat inFrame,int minSize,float minBox=250.0f);//处理视频帧得到前景目标的中心

private:
    ImageProcess *imageProcess;//图像处理类
    MyCascadeClassifier *classifier;//级联分类器
    FrameForeground *frameForeground;//前景检测类

private:

    bool isFirstRun;//第一次运行

    bool enableImageFilter;//是否进行图像滤波
    int filterType;//使用的滤波器
    int filterSize;//滤波器尺寸
    bool enableShowObject;//是否显示目标

    void init();

private:
    void saveConfig();
    void loadConfig();
};

#endif // OBJECTRECOGNITION_H
