#include "mycascadeclassifier.h"
#include <iostream>

MyCascadeClassifier::MyCascadeClassifier()
{
    init();
    std::cout << "MyCascadeClassifier()" << std::endl;
}

MyCascadeClassifier::~MyCascadeClassifier()
{
    std::cout << "~MyCascadeClassifier()"<< std::endl;
}

//初始化分类器
int MyCascadeClassifier::initClassifier(std::string cascadeName)
{
    isFirstRun=true;
    loadConfig();
    if (!cascade.load(cascadeName))
    {
        isClassifier = false;
        std::cout << "Error loading cascade:" << cascadeName << std::endl;
        return -30;
    }
    isClassifier = true;
    return 0;
}

//检测目标
std::vector<cv::Rect> MyCascadeClassifier::detectObjectRect(const cv::Mat& inFrame, int minSize, int maxSize)
{
    std::vector<cv::Rect> objects;
    cv::Mat frame_gray;
    objects.clear();
    if (inFrame.empty())
    {
        return objects;
    }
    if (isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if (isClassifier)
    {
        if (inFrame.channels() > 1)
        {
            cv::cvtColor(inFrame, frame_gray, cv::COLOR_BGR2GRAY);
        }
        else
        {
            frame_gray = inFrame;
        }
        if(enableEqualize)
        {
            cv::equalizeHist(frame_gray, frame_gray);
        }
        //-- Detect
        cascade.detectMultiScale(frame_gray, objects, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(minSize, minSize), cv::Size(maxSize, maxSize));
    }
    return objects;
}

void MyCascadeClassifier::init()
{
    isClassifier=false;
    isFirstRun=true;
    loadConfig();
}

void MyCascadeClassifier::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MyCascadeClassifier.xml", cv::FileStorage::WRITE,"utf-8");

    cv::write(fs,"enableEqualize", enableEqualize);

    fs.release();
}

void MyCascadeClassifier::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MyCascadeClassifier.xml", cv::FileStorage::READ,"utf-8");

    cv::read(fs["enableEqualize"], enableEqualize, false);

    fs.release();
}
