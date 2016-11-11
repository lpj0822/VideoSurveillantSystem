#include "objectrecognition.h"
#include <iostream>

ObjectRecognition::ObjectRecognition()
{
    init();
    std::cout << "ObjectRecognition()" << std::endl;
}

ObjectRecognition::~ObjectRecognition()
{
    if(imageProcess)
    {
        delete imageProcess;
        imageProcess = NULL;
    }
    if(classifier)
    {
        delete classifier;
        classifier = NULL;
    }
    if(frameForeground)
    {
        delete frameForeground;
        frameForeground = NULL;
    }
    std::cout << "~ObjectRecognition()" << std::endl;
}

void ObjectRecognition::initData(std::string classifierName)
{
    frameForeground->initData();
    classifier->initClassifier(classifierName);
    loadConfig();
}

std::vector<cv::Rect> ObjectRecognition::getFrameObejctdRect(const cv::Mat &inFrame, int minSize)
{
    std::vector<cv::Rect> objectRect;//目标的外部矩形边界
    objectRect.clear();
    if(inFrame.empty())
    {
        return objectRect;
    }
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }

    objectRect = classifier->detectObjectRect(inFrame, minSize);

    if(enableShowObject)
    {
        //显示
        imageProcess->drawRect(const_cast<cv::Mat&>(inFrame), objectRect, cv::Scalar( 255, 255, 255 ));
        cv::imshow("object", inFrame);
    }

    return objectRect;
}

//处理视频帧得到车辆目标
std::vector<cv::Rect> ObjectRecognition::getFrameCarObejctdRect(const cv::Mat& inFrame, int minSize, float minBox)
{
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat grayMat;
    std::vector<cv::Rect> objectRect;//目标的外部矩形边界
    objectRect.clear();

    if (inFrame.empty())
    {
        return objectRect;
    }
    if (isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }

    frameForeground->getFrameForeground(inFrame, fgMask, minBox);
    cv::cvtColor(inFrame,grayMat, cv::COLOR_BGR2GRAY);
    cv::bitwise_and(grayMat, fgMask, fgMask);
    objectRect = classifier->detectObjectRect(fgMask, minSize);

    if(enableShowObject)
    {
        //显示
        imageProcess->drawRect(const_cast<cv::Mat&>(inFrame), objectRect, cv::Scalar(255, 255, 255));
        cv::imshow("object", inFrame);
    }

    return objectRect;
}

//处理视频帧得到车辆目标
std::vector<cv::Point2f> ObjectRecognition::getFrameCarObejctdCenter(const cv::Mat& inFrame, int minSize, float minBox)
{
    std::vector<cv::Point2f> centers;//目标的中心点
    std::vector<cv::Rect> objectRect;//目标的外部矩形边界
    centers.clear();
    objectRect = getFrameCarObejctdRect(inFrame, minSize, minBox);
    if (inFrame.empty())
    {
        return centers;
    }
    for (int i = 0; i < objectRect.size(); i++)
    {
        centers.push_back((objectRect[i].br() + objectRect[i].tl()) * 0.5f);
    }
    if(enableShowObject)
    {
        //显示
        imageProcess->drawCenter(const_cast<cv::Mat&>(inFrame), centers, cv::Scalar(255, 255, 255));
        cv::imshow("object", inFrame);
    }

    return centers;
}

void ObjectRecognition::init()
{
    isFirstRun = true;
    imageProcess = new ImageProcess();//图像处理类
    classifier = new MyCascadeClassifier();//级联分类器
    frameForeground = new FrameForeground();//前景检测类
    loadConfig();
}

void ObjectRecognition::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ObjectRecognition.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs,"enableShowObject", enableShowObject);

    fs.release();
}

void ObjectRecognition::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ObjectRecognition.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["enableShowObject"], enableShowObject, true);

    fs.release();
}
