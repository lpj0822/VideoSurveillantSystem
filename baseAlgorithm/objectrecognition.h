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
    std::vector<cv::Rect> getFrameObejctdRect(const cv::Mat& inFrame, int minSize);
    //处理视频帧得到识别目标，得到目标的矩形轮廓
    std::vector<cv::Rect> getFrameCarObejctdRect(const cv::Mat& inFrame, int minSize, float minBox=250.0f);
    //处理视频帧得到识别目标，得到目标的中心
    std::vector<cv::Point2f> getFrameCarObejctdCenter(const cv::Mat& inFrame,int minSize, float minBox=250.0f);

private:
    ImageProcess *imageProcess;//图像处理类
    MyCascadeClassifier *classifier;//级联分类器
    FrameForeground *frameForeground;//前景检测类
    bool isFirstRun;//第一次运行

    bool enableShowObject;//是否显示目标

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // OBJECTRECOGNITION_H
