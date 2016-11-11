#include "frameforeground.h"
#include <iostream>

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
        bgs = NULL;
    }
    if(colorShadowDetector)
    {
        delete colorShadowDetector;
        colorShadowDetector = NULL;
    }
    if(imageFilter)
    {
        delete imageFilter;
        imageFilter = NULL;
    }
    std::cout << "~FrameForeground()" << std::endl;
}

//初始化参数
void FrameForeground::initData()
{
    isFirstRun=true;
    loadConfig();
    colorShadowDetector->initData();
    elementBGS = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * elementSizeBGS + 1, 2 * elementSizeBGS + 1),
                                           cv::Point(elementSizeBGS,elementSizeBGS));
    elementRemoveShadow = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * elementSizeRemoveShadow + 1, 2 * elementSizeRemoveShadow + 1),
                                              cv::Point(elementSizeRemoveShadow, elementSizeRemoveShadow));
    if(bgs)
    {
        delete bgs;
        bgs = NULL;
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
std::vector<cv::Rect> FrameForeground::getFrameForegroundRect(const cv::Mat& inFrame, float minBox)
{
    std::vector<cv::Rect> objectRect;//轮廓的外部矩形
    cv::Rect box;
    objectRect.clear();

    if (inFrame.empty())
    {
        return objectRect;
    }
    //计算轮廓
    calculateFrameForegroundContours(inFrame);
    for (int i = 0; i < objectContours.size(); i++)
    {
        box = cv::boundingRect(cv::Mat(objectContours[i]));
        if(box.area() > minBox)
        {
            objectRect.push_back(box);
        }
    }

    return objectRect;
}

//处理视频帧得到前景目标的中心
std::vector<cv::Point2f> FrameForeground::getFrameForegroundCenter(const cv::Mat& inFrame,float minBox)
{
    std::vector<cv::Point2f> centers;
    std::vector<cv::Rect> objectRect;
    centers.clear();
    if(inFrame.empty())
    {
        return centers;
    }
    objectRect = getFrameForegroundRect(inFrame, minBox);
    for (int loop = 0; loop < objectRect.size(); loop++)
    {
        centers.push_back((objectRect[loop].br() + objectRect[loop].tl()) * 0.5f);
    }
    return centers;
}

//处理视频帧得到目标的质心
std::vector<cv::Point2f> FrameForeground::getFrameForegroundCentroid(const cv::Mat& inFrame, float minBox)
{
    std::vector<cv::Point2f> centroid;//质心

    centroid.clear();

    if(inFrame.empty())
    {
        return centroid;
    }
    //计算轮廓
    calculateFrameForegroundContours(inFrame);
    for (int i = 0; i < objectContours.size(); i++)
    {

        cv::Moments mu = cv::moments(objectContours[i], false);
        if (mu.m00 > minBox)
        {
            centroid.push_back(cv::Point2f(static_cast<float>(mu.m10/mu.m00),
                                           static_cast<float>(mu.m01/mu.m00)));//质心
        }
    }

    return centroid;
}

//得到目标的多边形区域前景图像
void FrameForeground::getFrameForeground(const cv::Mat& inFrame, cv::Mat &foregroundFrame, float minBox)
{
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat img_bgmodel;

    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号
    std::vector<cv::Point> contours_hull;

    foregroundFrame=cv::Mat::zeros(inFrame.size(), CV_8UC1);

    if(inFrame.empty())
    {
        return;
    }

    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    getFrameForeground(inFrame, fgMask, img_bgmodel);
    //寻找轮廓
    cv::findContours(fgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    //得到轮廓的

    for(int i = 0; i < contours.size(); i++)
    {
        cv::convexHull(cv::Mat(contours[i]),contours_hull,true);
        //cv::approxPolyDP(cv::Mat(contours[i]), contours_hull, 3, true);
        if(cv::contourArea(contours_hull)>minBox)
        {
            cv::fillConvexPoly(foregroundFrame, contours_hull,cv::Scalar(255),8);
        }
    }
}

//得到前景图像
void FrameForeground::getFrameForeground(const cv::Mat& inFrame, cv::Mat& foregroundFrame, cv::Mat& backgroundFrame)
{
    cv::Mat filterMat;

    if(inFrame.empty())
    {
        return;
    }

    if(isFirstRun)
    {
        CV_Assert(bgs != NULL);
        saveConfig();
        isFirstRun = false;
    }
    //滤波
    if(enableImageFilter)
    {
        imageFilter->filter(inFrame, filterMat, imageFilterSize, imageFilterType);
    }
    else
    {
        filterMat = inFrame;
    }
    //背景分割
    bgs->process(filterMat, foregroundFrame, backgroundFrame);
    if (enableMorphology)
    {
        //cv::erode(foregroundFrame, foregroundFrame, elementBGS, cv::Point(-1, -1), 2);
        //cv::dilate(foregroundFrame, foregroundFrame, element, cv::Point(-1, -1), 2);
        cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_OPEN, elementBGS, cv::Point(-1,-1),1); //形态学操作，开运算
        cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_CLOSE, elementBGS);//闭运算
    }

    if(enableShowForeground)
    {
        //显示
        cv::imshow("filter frame",filterMat);
        cv::imshow(bgsName, foregroundFrame);
    }

}

//得到去除阴影的前景图像
void FrameForeground::getFrameRemoveShadowForeground(const cv::Mat& inFrame, cv::Mat& foregroundFrame)
{
    cv::Mat maskMat;
    cv::Mat img_bgmodel;

    if(inFrame.empty())
    {
        return;
    }
    //前景检测
    getFrameForeground(inFrame, maskMat, img_bgmodel);
    if (!maskMat.empty()&&!img_bgmodel.empty())
    {
        colorShadowDetector->removeShadows(inFrame,maskMat,img_bgmodel,foregroundFrame);

        if (enableMorphology)
        {
            //闭运算
            cv::morphologyEx(foregroundFrame, foregroundFrame, cv::MORPH_CLOSE, elementRemoveShadow, cv::Point(-1,-1), 1);
        }
    }

   if(enableShowForeground)
   {
       //显示
       cv::imshow("romoveShadow", foregroundFrame);
   }
}

//计算目标的多边形轮廓
void FrameForeground::calculateFrameForegroundContours(const cv::Mat& inFrame)
{
    cv::Mat fgMask; //fg mask fg mask generated
    cv::Mat edgeFrame;//边沿图像

    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号
    std::vector<cv::Point> contours_poly;
    objectContours.clear();
    if (inFrame.empty())
    {
        return;
    }
    //前景检测
    getFrameForeground(inFrame, fgMask);
    //使用Canny算子检测边缘
    cv::Canny(fgMask, edgeFrame, cannyThreshold,  cannyThreshold* 3, 3);
    //寻找轮廓
    cv::findContours(edgeFrame, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    //得到轮廓的
    for (int i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(contours[i]), contours_poly, 3, true);
        objectContours.push_back(contours_poly);
    }

    if(enableShowForeground)
    {
        //显示
        cv::imshow("edge frame", edgeFrame);
    }
}

void FrameForeground::init()
{
    bgs = NULL;//背景分割算法
    colorShadowDetector = new ColorShadowDetector();//阴影检测器
    imageFilter = new ImageFilter();//图像滤波
    isFirstRun = true;
    bgsName = "unKnowName";//背景分割算法名称
    objectContours.clear();

    loadConfig();
}

void FrameForeground::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/FrameForeground.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs, "flags", flags);
    cv::write(fs, "enableMorphology", enableMorphology);
    cv::write(fs, "elementSizeBGS", elementSizeBGS);
    cv::write(fs, "elementSizeRemoveShadow", elementSizeRemoveShadow);
    cv::write(fs, "cannyThreshold", cannyThreshold);
    cv::write(fs, "enableImageFilter", enableImageFilter);
    cv::write(fs, "imageFilterType", imageFilterType);
    cv::write(fs, "imageFilterSize", imageFilterSize);
    cv::write(fs, "enableShowForeground", enableShowForeground);

    fs.release();
}

void FrameForeground::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/FrameForeground.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["flags"], flags, 0);
    cv::read(fs["enableMorphology"], enableMorphology, true);
    cv::read(fs["elementSizeBGS"], elementSizeBGS, 2);
    cv::read(fs["elementSizeRemoveShadow"], elementSizeRemoveShadow, 2);
    cv::read(fs["cannyThreshold"], cannyThreshold, 10);
    cv::read(fs["enableImageFilter"], enableImageFilter, false);
    cv::read(fs["imageFilterType"], imageFilterType, 1);
    cv::read(fs["imageFilterSize"], imageFilterSize, 5);
    cv::read(fs["enableShowForeground"], enableShowForeground, true);

    fs.release();
}
