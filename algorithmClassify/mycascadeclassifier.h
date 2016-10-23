#ifndef MYCASCADECLASSIFIER_H
#define MYCASCADECLASSIFIER_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>

class MyCascadeClassifier
{
public:
    MyCascadeClassifier();
    ~MyCascadeClassifier();

    int initClassifier(std::string cascadeName);//初始化分类器

    std::vector<cv::Rect> detectObjectRect(cv::Mat inFrame,int minSize,int maxSize=1000);//检测目标
    std::vector<cv::Point2f> detectObjectCenter(cv::Mat inFrame,int minSize,int maxSize=1000);//检测目标

private:
    cv::CascadeClassifier cascade;//级联分类器
    bool isClassifier;//是否加载了分类器

    bool enableEqualize;//是否均衡化

    bool isFirstRun;

    void init();

private:
    void saveConfig();
    void loadConfig();
};

#endif // MYCASCADECLASSIFIER_H
