#include "objectrecognition.h"
#include <iostream>
#include <QDir>

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
        imageProcess=NULL;
    }
    if(classifier)
    {
        delete classifier;
        classifier=NULL;
    }
    if(frameForeground)
    {
        delete frameForeground;
        frameForeground=NULL;
    }
    std::cout << "~ObjectRecognition()" << std::endl;
}

void ObjectRecognition::initData(std::string classifierName)
{
    frameForeground->initData();
    classifier->initClassifier(classifierName);
    loadConfig();
}

//处理视频帧得到车辆目标
std::vector<cv::Rect> ObjectRecognition::getFrameCarObejctdRect(cv::Mat inFrame, int minSize,float minBox)
{
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat grayMat;

    std::vector<cv::Rect> objectRect;//轮廓的外部矩形边界

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

    frameForeground->getFrameForeground(inFrame,fgMask,minBox);
    cv::cvtColor(inFrame,grayMat,cv::COLOR_BGR2GRAY);
    cv::bitwise_and(grayMat,fgMask,fgMask);
    objectRect=classifier->detectObjectRect(fgMask,minSize);

    if(enableShowObject)
    {
        //显示
        imageProcess->drawRect(inFrame,objectRect,cv::Scalar( 255, 255, 255 ));
        cv::imshow("object", inFrame);
    }

    return objectRect;
}

//处理视频帧得到车辆目标
std::vector<cv::Point2f> ObjectRecognition::getFrameCarObejctdCenter(cv::Mat inFrame,int minSize,float minBox)
{
    std::vector<cv::Point2f> centers;
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat grayMat;

    centers.clear();

    if(inFrame.empty())
    {
        return centers;
    }
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }

    frameForeground->getFrameForeground(inFrame,fgMask,minBox);
    cv::cvtColor(inFrame,grayMat,cv::COLOR_BGR2GRAY);
    cv::bitwise_and(grayMat,fgMask,fgMask);
    centers=classifier->detectObjectCenter(fgMask,minSize);

    if(enableShowObject)
    {
        //显示
        imageProcess->drawCenter(inFrame,centers,cv::Scalar( 255, 255, 255 ));
        cv::imshow("object", inFrame);
    }

    return centers;
}

void ObjectRecognition::init()
{
    isFirstRun=true;
    imageProcess=new ImageProcess();//图像处理类
    classifier=new MyCascadeClassifier();//级联分类器
    frameForeground=new FrameForeground();//前景检测类
    loadConfig();
}

void ObjectRecognition::saveConfig()
{
    cv::FileStorage fs;
    QDir makeDir;
    if(!makeDir.exists("./config/"))
    {
        if(!makeDir.mkdir("./config/"))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return;
        }
    }
    fs.open("./config/ObjectRecognition.xml",cv::FileStorage::WRITE,"utf-8");

    cv::write(fs, "enableImageFilter", enableImageFilter);
    cv::write(fs, "filterType", filterType);
    cv::write(fs, "filterSize", filterSize);
    cv::write(fs,"enableShowObject",enableShowObject);

    fs.release();
}

void ObjectRecognition::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ObjectRecognition.xml",cv::FileStorage::READ,"utf-8");

    cv::read(fs["enableImageFilter"], enableImageFilter,false);
    cv::read(fs["filterType"], filterType,1);
    cv::read(fs["filterSize"],filterSize,5);
    cv::read(fs["enableShowObject"],enableShowObject,false);

    fs.release();
}
