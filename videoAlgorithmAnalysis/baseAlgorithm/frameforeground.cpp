#include "frameforeground.h"
#include <iostream>
#include <QDir>

FrameForeground::FrameForeground()
{
    init();
    std::cout << "FrameForeground()" << std::endl;
}

FrameForeground::~FrameForeground()
{
    if(bgs)
    {
        delete bgs;
        bgs=NULL;
    }
    if(colorShadowDetector)
    {
        delete colorShadowDetector;
        colorShadowDetector=NULL;
    }
    if(imageFilter)
    {
        delete imageFilter;
        imageFilter=NULL;
    }
    std::cout << "~FrameForeground()" << std::endl;
}

//初始化参数
void FrameForeground::initData()
{
    isFirstRun=true;
    loadConfig();
    colorShadowDetector->initData();
    elementBGS = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * elementSizeBGS + 1, 2 * elementSizeBGS + 1),cv::Point(elementSizeBGS,elementSizeBGS));
    elementRemove = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * elementSizeRemove + 1, 2 * elementSizeRemove + 1),cv::Point(elementSizeRemove,elementSizeRemove));
    if(bgs)
    {
        delete bgs;
        bgs=NULL;
    }
    switch(flags)
    {
    case 0:
        bgs=new MixtureOfGaussianV2BGS();
        bgsName="MixtureOfGaussianV2BGS";
        break;
    case 1:
        bgs=new PixelBasedAdaptiveSegmenter();
        bgsName="PixelBasedAdaptiveSegmenter";
        break;
    case 2:
        bgs=new VibeBackgroundSegmenter();
        bgsName="VibeBackgroundSubtrack";
        break;
    default:
        bgs=new MixtureOfGaussianV2BGS();
        bgsName="MixtureOfGaussianV2BGS";
        break;
    }
}

//处理视频帧得到前景目标
std::vector<cv::Rect> FrameForeground::getFrameForegroundRect(cv::Mat inFrame,float minBox)
{
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat edgeFrame;//边沿图像


    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号

    std::vector<cv::Rect> objectRect;//轮廓的外部矩形边界
    std::vector< std::vector<cv::Point> > objectContours;//人的轮廓

    cv::Rect box;
    std::vector<cv::Point> contours_poly;

    objectRect.clear();

    if(inFrame.empty())
    {
        return objectRect;
    }

    getFrameForeground(inFrame,fgMask);
    if(bgs)
    {
        //使用Canny算子检测边缘
        cv::Canny(fgMask, edgeFrame, cannyThreshold,  cannyThreshold* 3, 3);
        //寻找轮廓
        cv::findContours(edgeFrame, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        //得到轮廓的
        for(int i = 0; i < contours.size(); i++)
        {
            cv::approxPolyDP(cv::Mat(contours[i]), contours_poly, 3, true);
            box = cv::boundingRect(cv::Mat(contours_poly));
            if(box.area()>minBox)
            {
                objectContours.push_back(contours[i]);
                objectRect.push_back(box);
            }
        }

        if(enableShowForeground)
        {
            //显示
            cv::imshow("edge frame", edgeFrame);
        }
    }

    return objectRect;
}

//处理视频帧得到前景目标的中心
std::vector<cv::Point2f> FrameForeground::getFrameForegroundCenter(cv::Mat inFrame,float minBox)
{
    std::vector<cv::Point2f> centers;

    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat edgeFrame;//边沿图像


    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号

    cv::Rect box;
    std::vector<cv::Point> contours_poly;

    centers.clear();

    if(inFrame.empty())
    {
        return centers;
    }

    getFrameForeground(inFrame,fgMask);
    if(bgs)
    {
        //使用Canny算子检测边缘
        cv::Canny(fgMask, edgeFrame, cannyThreshold,  cannyThreshold* 3, 3);
        //寻找轮廓
        cv::findContours(edgeFrame, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        //得到轮廓的
        for(int i = 0; i < contours.size(); i++)
        {
            cv::approxPolyDP(cv::Mat(contours[i]), contours_poly, 3, true);
            box = cv::boundingRect(cv::Mat(contours_poly));
            if(box.area()>minBox)
            {
                centers.push_back((box.br() + box.tl())*0.5f);
            }
        }

        if(enableShowForeground)
        {
            //显示
            cv::imshow("edge frame", edgeFrame);
        }
    }

    return centers;
}

//处理视频帧得到目标的质心
std::vector<cv::Point2f> FrameForeground::getFrameForegroundCentroid(cv::Mat inFrame,float minBox)
{
    std::vector<cv::Point2f> centroid;//质心

    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat edgeFrame;//边沿图像


    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号

    std::vector<cv::Point> contours_poly;

    centroid.clear();

    if(inFrame.empty())
    {
        return centroid;
    }

    getFrameForeground(inFrame,fgMask);
    if(bgs)
    {
        //使用Canny算子检测边缘
        cv::Canny(fgMask, edgeFrame, cannyThreshold,  cannyThreshold* 3, 3);
        //寻找轮廓
        cv::findContours(edgeFrame, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        //得到轮廓的
        for(int i = 0; i < contours.size(); i++)
        {
            cv::approxPolyDP(cv::Mat(contours[i]), contours_poly, 3, true);
            cv::Moments mu=cv::moments(contours_poly,false);
            if(mu.m00>minBox)
            {
                centroid.push_back(cv::Point2f(static_cast<float>(mu.m10/mu.m00),static_cast<float>(mu.m01/mu.m00)));//质心
            }
        }

        if(enableShowForeground)
        {
            //显示
            cv::imshow("edge frame", edgeFrame);
        }

    }

    return centroid;
}

//得到前景图像
void FrameForeground::getFrameForeground(cv::Mat inFrame,cv::Mat& foregroundFrame)
{
    cv::Mat img_bgmodel;
    cv::Mat filterMat;

    if(inFrame.empty())
    {
        return;
    }

    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if(bgs)
    {
        //滤波
        if(enableImageFilter)
        {
            imageFilter->filter(inFrame,filterMat,filterSize,filterType);
        }
        else
        {
            filterMat=inFrame;
        }
        //update the background model
        //tic();
        bgs->process(filterMat,foregroundFrame,img_bgmodel);//背景分割
        //toc();
        //cv::erode(fgMask, fgMask, elementBGS, cv::Point(-1, -1), 2);
        //cv::dilate(fgMask, fgMask, element, cv::Point(-1, -1), 2);
        cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_OPEN,elementBGS,cv::Point(-1,-1),1); //形态学操作，开运算
        cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_CLOSE, elementBGS);//闭运算

        if(enableShowForeground)
        {
            //显示
            cv::imshow("filter frame",filterMat);
            cv::imshow(bgsName, foregroundFrame);
        }
    }
}

//得到去除阴影的前景图像
void FrameForeground::getFrameRemoveShadowForeground(cv::Mat inFrame,cv::Mat& foregroundFrame)
{
    cv::Mat img_bgmodel;
    cv::Mat filterMat;
    cv::Mat maskMat;

    if(inFrame.empty())
    {
        return;
    }

    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if(bgs)
    {
        //滤波
        if(enableImageFilter)
        {
            imageFilter->filter(inFrame,filterMat,filterSize,filterType);
        }
        else
        {
            filterMat=inFrame;
        }
        //update the background model
        //tic();
        bgs->process(filterMat,maskMat,img_bgmodel);//背景分割
        //toc();
        //cv::erode(fgMask, fgMask, elementBGS, cv::Point(-1, -1), 2);
        //cv::dilate(fgMask, fgMask, element, cv::Point(-1, -1), 2);
        cv::morphologyEx(maskMat, maskMat, cv::MORPH_OPEN,elementBGS,cv::Point(-1,-1),1); //形态学操作，开运算
        cv::morphologyEx(maskMat, maskMat, cv::MORPH_CLOSE, elementBGS);//闭运算

        if(!maskMat.empty()&&!img_bgmodel.empty())
        {
            colorShadowDetector->removeShadows(inFrame,maskMat,img_bgmodel,foregroundFrame);
            cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_CLOSE, elementRemove,cv::Point(-1,-1),1);//闭运算
        }

        if(enableShowForeground)
        {
            //显示
            cv::imshow("filter frame",filterMat);
            cv::imshow(bgsName, maskMat);
            cv::imshow("romoveShadow",foregroundFrame);
        }
    }
}

//得到目标的多边形区域前景图像
void FrameForeground::getFrameForeground(cv::Mat inFrame, cv::Mat &foregroundFrame,float minBox)
{
    cv::Mat fgMask; //fg mask fg mask generated
    //cv::Mat edgeFrame;//边沿图像
    cv::Mat img_bgmodel;
    cv::Mat filterMat;

    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号

    std::vector<cv::Point> contours_hull;

    foregroundFrame=cv::Mat::zeros(inFrame.size(),CV_8UC1);

    if(inFrame.empty())
    {
        return;
    }

    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if(bgs)
    {
        //滤波
        if(enableImageFilter)
        {
            imageFilter->filter(inFrame,filterMat,filterSize,filterType);
        }
        else
        {
            filterMat=inFrame;
        }
        //update the background model
        //tic();
        bgs->process(filterMat,fgMask,img_bgmodel);//背景分割
        //toc();

        //cv::erode(fgMask, fgMask, elementBGS, cv::Point(-1, -1), 2);
        //cv::dilate(fgMask, fgMask, elementBGS, cv::Point(-1, -1), 2);
        cv::morphologyEx(fgMask, fgMask, cv::MORPH_OPEN,elementBGS,cv::Point(-1,-1),1); //形态学操作，开运算
        cv::dilate(fgMask, fgMask, elementBGS, cv::Point(-1, -1), 2);
        //使用Canny算子检测边缘
        //cv::Canny(fgMask, edgeFrame, cannyThreshold,  cannyThreshold* 3, 3);
        //寻找轮廓
        cv::findContours(fgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        //得到轮廓的

        for(int i = 0; i < contours.size(); i++)
        {
            cv::convexHull(cv::Mat(contours[i]),contours_hull,true);
            //cv::approxPolyDP(cv::Mat(contours[i]), contours_hull, 3, true);
            if(cv::contourArea(contours_hull)>minBox)
            {
                cv::fillConvexPoly(foregroundFrame,contours_hull,cv::Scalar(255),8);
            }
        }

        if(enableShowForeground)
        {
            //显示
            imshow("filter frame",filterMat);
            //imshow("edge frame", edgeFrame);
            imshow(bgsName, fgMask);
        }
    }
}

void FrameForeground::saveConfig()
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
    fs.open("./config/FrameForeground.xml",cv::FileStorage::WRITE,"utf-8");

    cv::write(fs, "flags", flags);
    cv::write(fs, "elementSizeBGS", elementSizeBGS);
    cv::write(fs,"elementSizeRemove",elementSizeRemove);
    cv::write(fs,"cannyThreshold",cannyThreshold);
    cv::write(fs, "enableImageFilter", enableImageFilter);
    cv::write(fs, "filterType", filterType);
    cv::write(fs, "filterSize", filterSize);
    cv::write(fs, "enableShowForeground", enableShowForeground);

    fs.release();
}

void FrameForeground::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/FrameForeground.xml",cv::FileStorage::READ,"utf-8");

    cv::read(fs["flags"], flags,0);
    cv::read(fs["elementSizeBGS"],elementSizeBGS,2);
    cv::read(fs["elementSizeRemove"],elementSizeRemove,2);
    cv::read(fs["cannyThreshold"],cannyThreshold,10);
    cv::read(fs["enableImageFilter"], enableImageFilter,false);
    cv::read(fs["filterType"], filterType,1);
    cv::read(fs["filterSize"],filterSize,5);
    cv::read(fs["enableShowForeground"], enableShowForeground,true);

    fs.release();
}

void FrameForeground::init()
{
    bgs=NULL;//背景分割算法
    colorShadowDetector=new ColorShadowDetector();//阴影检测器
    imageFilter=new ImageFilter();//图像滤波
    isFirstRun=true;
    bgsName="unKnowName";//背景分割算法名称

    loadConfig();
}
