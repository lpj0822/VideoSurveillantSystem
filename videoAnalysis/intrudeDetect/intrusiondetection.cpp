#include "intrusiondetection.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <iostream>

IntrusionDetection::IntrusionDetection(QObject *parent) : QObject(parent)
{
    initData();
    std::cout << "IntrusionDetection()" << std::endl;
}

IntrusionDetection::~IntrusionDetection()
{
    if(imageProcess)
    {
        delete imageProcess;//图像处理类
        imageProcess = NULL;//图像处理类
    }
    if(geometryCalculations)
    {
        delete geometryCalculations;
        geometryCalculations = NULL;
    }
    if(frameForeground)
    {
        delete frameForeground;
        frameForeground = NULL;
    }
    std::cout << "~IntrusionDetection()" << std::endl;
}

//入侵检测
int IntrusionDetection::detect(const cv::Mat &frame)
{
    cv::Mat roiArea;//区域截图
    int objectNumber = 0;
    std::vector<cv::Rect> objectRect;//检测的目标矩形边界
    currentDate = QDateTime::currentDateTime().toString("yyyy:MM:dd-hh:mm:ss");
    int isIntrusion = 0;
    errorCode = 0;
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun = false;
    }
    if(!frame.empty())
    {
        for(int loop=0; loop<(int)detectArea.size(); loop++)
        {
            objectNumber = 0;
            roiArea = frame(detectArea[loop].getPolygonRect());
            objectRect.clear();
            objectRect = detectObject(roiArea);
            matchDetectArea(objectRect,loop);
            isIntrusion = intrusionArea(loop);
            objectNumber += (int)objectRect.size();
            if(errorCode<0)
            {
                return errorCode;
            }
            else
            {
                return isIntrusion;
            }
        }
        //绘制入侵区域
        //drawingDetectArea(frame,cv::Scalar(255,0,0));
        //绘制当前时间
        //cv::putText(frame, currentDate.toStdString().c_str(), cv::Point(15, 15),cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
    }

    return 0;
}

//初始化检测参数
void IntrusionDetection::initDetectData()
{
    isFirstRun = true;
    errorCode = 0;
    detectArea.clear();

    loadConfig();
    frameForeground->initData();
    IntrusionArea area;
    int number = (int)pointsArea.size();
    for(int loop1=0; loop1<number; loop1++)
    {
        area.setPolygon(pointsArea[loop1]);
        area.setIntrusionCount(0);
        area.isIntrusion = false;
        detectArea.push_back(area);
    }
}

//对某一个区域匹配目标
void IntrusionDetection::matchDetectArea(std::vector<cv::Rect> vectorRect, int number)
{
    int numberRect = (int)vectorRect.size();
    float maxCrossValue = 0.0f;
    float intersectionValue= 0.0f;//两个矩形的相交度
    std::vector<cv::Point> rectPolygon;
    rectPolygon.clear();
    for(int loop=0; loop<numberRect;loop++)
    {
        rectPolygon.clear();
        rectPolygon = geometryCalculations->covRectPolygon(vectorRect[loop]);//矩形转多边形表示
        //得到两多边形的相交度
        intersectionValue = geometryCalculations->polygonOverlap(detectArea[number].getPolygon1(), rectPolygon);
        //qDebug()<<"crossvalue:"<<intersectionValue;
        if(intersectionValue > maxCrossValue)
        {
            maxCrossValue = intersectionValue;
        }
    }
    qDebug()<<"maxCrossValue:"<<maxCrossValue;
    if (maxCrossValue > crossMatchMaxValue)
    {
        detectArea[number].addIntrusionCount(1);
        detectArea[number].isIntrusion = true;
    }
    else
    {
        detectArea[number].isIntrusion = false;
    }
}

//判断某个区域是否入侵
int  IntrusionDetection::intrusionArea(int number)
{
    if(detectArea[number].isIntrusion)
    {
        detectArea[number].isIntrusion = false;
        return number + 1;
    }
    else
    {
        return 0;
    }
}

//检测运动目标
std::vector<cv::Rect> IntrusionDetection::detectObject(const cv::Mat &frame)
{
    std::vector<cv::Rect> objectRect;//检测的目标矩形边界
    objectRect.clear();
    if(frameForeground)
    {
        objectRect = frameForeground->getFrameForegroundRect(frame, minBox);
        if(isDrawObject)
        {
            imageProcess->drawRect(const_cast<cv::Mat&>(frame), objectRect, cv::Scalar(0,0,255));
        }
    }

    return objectRect;
}

//得到错误码
int IntrusionDetection::getErrorCode()
{
    return errorCode;
}

//得到检测区域
std::vector<IntrusionArea>&  IntrusionDetection::getDetectArea()
{
    return detectArea;
}

//初始化数据
void IntrusionDetection::initData()
{
    errorCode=0;
    isFirstRun=true;

    detectArea.clear();
    pointsArea.clear();

    imageProcess = new ImageProcess();//图像处理类
    geometryCalculations = new GeometryCalculations();//几何运算类
    frameForeground = new FrameForeground();//前景检测类

    loadConfig();
}

void IntrusionDetection::saveConfig()
{
    cv::FileStorage fs;
    QDir makeDir;
    if(!makeDir.exists("./config/"))
    {
        if(!makeDir.mkdir("./config/"))
        {
            std::cout << "make dir fail!" << std::endl;
            return;
        }
    }
    fs.open("./config/IntrusionDetection.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs,"isDrawObject",isDrawObject);
    cv::write(fs, "crossMatchMaxValue", crossMatchMaxValue);
    cv::write(fs, "minBox", minBox);
    for(int loop=0;loop<(int)pointsArea.size();loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(), pointsArea[loop]);
    }
    fs.release();
}

void IntrusionDetection::loadConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    pointsArea.clear();
    fs.open("./config/IntrusionDetection.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["isDrawObject"], isDrawObject, false);
    cv::read(fs["crossMatchMaxValue"], crossMatchMaxValue, 0.3f);
    cv::read(fs["minBox"], minBox, 300);

    cv::FileNode node = fs["pointsArea0"];
    if(node.isNone())
    {
        return;
    }
    cv::FileNodeIterator iterator = node.begin(), iterator_end = node.end();
    for(int loop=0; iterator!=iterator_end; iterator++, loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        if(fs[tempName.toStdString().c_str()].isNone())
            break;
        cv::read(fs[tempName.toStdString().c_str()], tempVector);
        pointsArea.push_back(tempVector);
    }

    fs.release();
}

