#include "vehiclecouting.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <iostream>

VehicleCouting::VehicleCouting(QObject *parent) : QObject(parent)
{
    init();
    std::cout << "VehicleCouting()" << std::endl;
}

VehicleCouting::~VehicleCouting()
{
    if (imageProcess)
    {
        delete imageProcess;
        imageProcess = NULL;
    }
    if (geometryCalculations)
    {
        delete geometryCalculations;
        geometryCalculations = NULL;
    }
    if (frameForeground)
    {
        delete frameForeground;
        frameForeground = NULL;
    }
    for (int loop1=0;loop1<(int)allMultipleTrackers.size();loop1++)
    {
        if (allMultipleTrackers[loop1])
        {
            delete allMultipleTrackers[loop1];
            allMultipleTrackers[loop1] = NULL;
        }
    }
    std::cout << "~VehicleCouting()" << std::endl;
}

//车辆逆行检测
int VehicleCouting::detect(const cv::Mat& frame)
{
    cv::Mat roiArea;//区域截图
    std::vector<cv::Point2f> objectCenter;//检测的目标中心
    currentDate = QDateTime::currentDateTime().toString("yyyy:MM:dd-hh:mm:ss");
    int counting = 0;
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
            if(detectArea[loop].getNormalDirection()>0)
            {
                roiArea = frame(detectArea[loop].getPolygonRect());
                objectCenter.clear();
                objectCenter = detectObjectCenter(roiArea, loop);
                tracking(roiArea, objectCenter, loop);
                countingVehicle(loop);
                counting = detectArea[loop].getVehicleCouting();
                if(errorCode < 0)
                {
                    return errorCode;
                }
                else
                {
                    return counting;
               }
            }
        }
        //绘制检测区域
        //drawingDetectArea(frame,cv::Scalar(255,0,0));
        //绘制当前时间
        //cv::putText(frame, currentDate.toStdString().c_str(), cv::Point(15, 15),cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
    }
    return counting;

}

//检测运动目标得到运动目标中心
std::vector<cv::Point2f> VehicleCouting::detectObjectCenter(const cv::Mat &frame, int number)
{
    std::vector<cv::Point2f> myCenterPoint;
    std::vector<cv::Point2f> objectCenters;//检测目标的中心
    objectCenters.clear();
    myCenterPoint.clear();
    objectCenters = frameForeground->getFrameForegroundCentroid(frame, minBox);
    //objectCenters = frameForeground->getFrameForegroundCenter(frame, minBox);
    int count= static_cast<int>(objectCenters.size());
    for (int loop=0; loop<count; loop++)
    {
        if(geometryCalculations->pointInPolygon(objectCenters[loop], detectArea[number].getPolygon1()))
        {
            myCenterPoint.push_back(objectCenters[loop]);
        }
    }
    if (isDrawObject)
    {
        imageProcess->drawCenter(const_cast<cv::Mat&>(frame), myCenterPoint, cv::Scalar(0,255,255));
    }

    return myCenterPoint;
}

//对目标进行多目标跟踪
void VehicleCouting::tracking(const cv::Mat& roi, const std::vector<cv::Point2f>& centers, int number)
{
    if(allMultipleTrackers[number])
    {
        allMultipleTrackers[number]->mutilpleTracking(roi, centers);
    }
}

//统计车流量
void VehicleCouting::countingVehicle(int number)
{
    int direction = detectArea[number].getNormalDirection();
    int position = detectArea[number].getMedianPoint();
    std::vector<KalmanTracker*> trackers = allMultipleTrackers[number]->getListTrackers();
    int count = allMultipleTrackers[number]->getTrackersCount();
    std::cout << "tracker number:"<< count << std::endl;
    for(int loop=0; loop< count; loop++)
    {
        std::vector<cv::Point2f> points = trackers[loop]->getTrace();
        int size = static_cast<int>(points.size());
        if(size > 1)
        {
            cv::Point2f currentPoint = points[size-1];
            cv::Point2f prePoint = points[size-2];
            switch(direction)
            {
            case 1:
                if(currentPoint.y <= position && prePoint.y > position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 2:
                if(currentPoint.y >= position && prePoint.y < position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 3:
                if(currentPoint.x <= position && prePoint.x>position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 4:
                if(currentPoint.x >= position&&prePoint.x < position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            default:
                break;
            }
        }
    }
}

//每个跟踪区域重新开始跟踪
void VehicleCouting::startTrcaking(bool isStart)
{
    for (int loop=0; loop<(int)allMultipleTrackers.size(); loop++)
    {
        if (allMultipleTrackers[loop])
        {
            allMultipleTrackers[loop]->initStart(isStart);
        }
    }
}

//初始化检测参数
void VehicleCouting::initDetectData()
{
    isFirstRun = true;
    errorCode = 0;
    //加载参数
    loadConfig();
    frameForeground->initData();
    initData();
}

void VehicleCouting::initData()
{
    detectArea.clear();
    for (int loop1=0; loop1<(int)allMultipleTrackers.size(); loop1++)
    {
        if (allMultipleTrackers[loop1])
        {
            delete allMultipleTrackers[loop1];
            allMultipleTrackers[loop1] = NULL;
        }
    }
    allMultipleTrackers.clear();

    CountingArea area;
    int number = (int)pointsArea.size();
    for(int loop=0; loop<number; loop++)
    {
        area.setPolygon(pointsArea[loop]);
        area.setNormalDirection(areaDirection[loop]);
        area.setVehicleCouting(0);
        area.setMedianPoint(medianLine.getMedianPosition(pointsArea[loop], areaDirection[loop]));
        detectArea.push_back(area);
        KalmanMultipleTracker* tarcker = new KalmanMultipleTracker();
        allMultipleTrackers.push_back(tarcker);
    }
}

//得到错误码
int VehicleCouting::getErrorCode()
{
    return errorCode;
}

//得到检测区域
std::vector<CountingArea>&  VehicleCouting::getDetectArea()
{
    return detectArea;
}

//初始化数据
void VehicleCouting::init()
{
    errorCode = 0;
    isFirstRun = true;

    detectArea.clear();
    pointsArea.clear();
    areaDirection.clear();
    allMultipleTrackers.clear();

    imageProcess = new ImageProcess();//图像处理算法类
    geometryCalculations = new GeometryCalculations();//几何运算类
    frameForeground = new FrameForeground();//前景检测类

    loadConfig();
}

void VehicleCouting::saveConfig()
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
    fs.open("./config/VehicleCouting.xml", cv::FileStorage::WRITE, "utf-8");

    cv::write(fs,"isDrawObject", isDrawObject);
    cv::write(fs, "minBox", minBox);
    cv::write(fs, "areaDirection", areaDirection);
    for(int loop=0; loop<(int)pointsArea.size(); loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(), pointsArea[loop]);
    }
    fs.release();
}

void VehicleCouting::loadConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    pointsArea.clear();
    areaDirection.clear();
    fs.open("./config/VehicleCouting.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["isDrawObject"],isDrawObject, false);
    cv::read(fs["minBox"], minBox, 300.0f);
    cv::read(fs["areaDirection"], areaDirection);

    cv::FileNode node = fs["pointsArea0"];
    if(node.isNone())
    {
        return;
    }
    cv::FileNodeIterator iterator = node.begin(), iterator_end = node.end();
    for(int loop=0; iterator != iterator_end; iterator++,loop++)
    {
        QString tempName = "pointsArea" + QString::number(loop);
        if(fs[tempName.toStdString().c_str()].isNone())
            break;
        cv::read(fs[tempName.toStdString().c_str()], tempVector);
        pointsArea.push_back(tempVector);
    }

    fs.release();
}
