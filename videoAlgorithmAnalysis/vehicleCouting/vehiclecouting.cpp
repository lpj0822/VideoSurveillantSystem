#include "vehiclecouting.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>

VehicleCouting::VehicleCouting(QObject *parent) : QObject(parent)
{
    init();
    std::cout<<"VehicleCouting()"<<std::endl;
}

VehicleCouting::~VehicleCouting()
{
    if(frameForeground)
    {
        delete frameForeground;
        frameForeground=NULL;
    }
    for(int loop1=0;loop1<(int)allTracker.size();loop1++)
    {
        if(allTracker[loop1])
        {
            delete allTracker[loop1];
            allTracker[loop1]=NULL;
        }
    }
    std::cout<<"~VehicleCouting()"<<std::endl;
}

//车辆逆行检测
int VehicleCouting::detect(cv::Mat &frame)
{
    cv::Mat roiArea;//区域截图
    currentDate = QDateTime::currentDateTime().toString("yyyy:MM:dd-hh:mm:ss");
    int counting=0;
    errorCode=0;
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if(!frame.empty())
    {
        for(int loop=0;loop<(int)detectArea.size();loop++)
        {
            if(detectArea[loop].getNormalDirection()>0)
            {
                roiArea=frame(detectArea[loop].getPolygonRect());
                tracking(roiArea,loop);
                countingVehicle(loop);
                counting=detectArea[loop].getVehicleCouting();
                if(errorCode<0)
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

//对目标进行多目标跟踪
void VehicleCouting::tracking(cv::Mat& roi,int number)
{
    cv::Mat fsMak;
    if(frameForeground)
    {
        frameForeground->getFrameForeground(roi,fsMak);
        allTracker[number]->process(roi,fsMak);
    }
}

//统计车流量
void VehicleCouting::countingVehicle(int number)
{
    int direction=detectArea[number].getNormalDirection();
    int position=detectArea[number].getMedianPoint();
    std::vector< std::vector<cv::Point2d> > tracks=allTracker[number]->getTracksPoints();
    for(int loop=0;loop<(int) tracks.size();loop++)
    {
        std::vector<cv::Point2d> points=tracks[loop];
        int size=(int)points.size();
        if(size>1)
        {
            cv::Point2d currentPoint=points[size-1];
            cv::Point2d prePoint=points[size-2];
            switch(direction)
            {
            case 1:
                if(currentPoint.y<=position&&prePoint.y>position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 2:
                if(currentPoint.y>=position&&prePoint.y<position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 3:
                if(currentPoint.x<=position&&prePoint.x>position)
                {
                    detectArea[number].addVehicleCouting(1);
                }
                break;
            case 4:
                if(currentPoint.x>=position&&prePoint.x<position)
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
    for(int loop1=0;loop1<(int)allTracker.size();loop1++)
    {
        if(allTracker[loop1])
        {
            allTracker[loop1]->initStart(isStart);
        }
    }
}

//初始化检测参数
void VehicleCouting::initDetectData()
{
    isFirstRun=true;
    errorCode=0;
    //加载参数
    loadConfig();
    frameForeground->initData();
    initData();
}

void VehicleCouting::initData()
{
    detectArea.clear();
    for(int loop1=0;loop1<(int)allTracker.size();loop1++)
    {
        if(allTracker[loop1])
        {
            delete allTracker[loop1];
            allTracker[loop1]=NULL;
        }
    }
    allTracker.clear();

    CountingArea area;
    int number=(int)pointsArea.size();
    for(int loop=0;loop<number;loop++)
    {
        area.setPolygon(pointsArea[loop]);
        area.setNormalDirection(areaDirection[loop]);
        area.setVehicleCouting(0);
        area.setMedianPoint(medianLine.getMedianPosition(pointsArea[loop],areaDirection[loop]));
        detectArea.push_back(area);
        BlobMultipleTracker* tarcker=new BlobMultipleTracker();
        allTracker.push_back(tarcker);
    }
}

//绘制检测区域
void VehicleCouting::drawingDetectArea(cv::Mat &inFrame ,cv::Scalar color)
{
    int number=(int)detectArea.size();
    int index=0;
    std::vector<cv::Point> polygon;
    polygon.clear();
    for(int loop1=0;loop1<number;loop1++)
    {
        polygon=detectArea[loop1].getPolygon();
        int num=(int)polygon.size();
        for(int loop2=0;loop2<num;loop2++)
        {
            index=(loop2+1)%num;
            line(inFrame,polygon[loop2],polygon[index],color,2,8);
        }
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
    errorCode=0;
    isFirstRun=true;

    detectArea.clear();
    pointsArea.clear();
    areaDirection.clear();
    allTracker.clear();

    frameForeground=new FrameForeground();//前景检测类

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
            std::cout<<"make dir fail!"<<std::endl;
            return;
        }
    }
    fs.open("./config/VehicleCouting.xml",cv::FileStorage::WRITE,"utf-8");

    cv::write(fs,"areaDirection",areaDirection);
    for(int loop=0;loop<(int)pointsArea.size();loop++)
    {
        QString tempName="pointsArea"+QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(),pointsArea[loop]);
    }
    fs.release();
}

void VehicleCouting::loadConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    pointsArea.clear();
    areaDirection.clear();
    fs.open("./config/VehicleCouting.xml",cv::FileStorage::READ,"utf-8");

    cv::read(fs["areaDirection"],areaDirection);

    cv::FileNode node=fs["pointsArea0"];
    if(node.isNone())
    {
        return;
    }
    cv::FileNodeIterator iterator=node.begin(),iterator_end=node.end();
    for(int loop=0;iterator!=iterator_end;iterator++,loop++)
    {
        QString tempName="pointsArea"+QString::number(loop);
        if(fs[tempName.toStdString().c_str()].isNone())
            break;
        cv::read(fs[tempName.toStdString().c_str()],tempVector);
        pointsArea.push_back(tempVector);
    }

    fs.release();
}
