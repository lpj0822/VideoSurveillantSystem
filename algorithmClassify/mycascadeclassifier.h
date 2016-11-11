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
    //初始化分类器
    int initClassifier(std::string cascadeName);
    //识别目标
    std::vector<cv::Rect> detectObjectRect(const cv::Mat& inFrame, int minSize, int maxSize=1000);

private:
    cv::CascadeClassifier cascade;//级联分类器
    bool isClassifier;//是否加载了分类器
    bool isFirstRun;

    bool enableEqualize;//是否均衡化

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // MYCASCADECLASSIFIER_H
