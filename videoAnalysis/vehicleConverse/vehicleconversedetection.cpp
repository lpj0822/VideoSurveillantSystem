#include "vehicleconversedetection.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>

VehicleConverseDetection::VehicleConverseDetection(QObject *parent) : QObject(parent)
{
    init();
    std::cout << "VehicleConverseDetection()" << std::endl;
}

VehicleConverseDetection::~VehicleConverseDetection()
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
    if (objectRecognition)
    {
        delete objectRecognition;
        objectRecognition = NULL;
    }
    for (int loop1=0;loop1<(int)allMultipleTrackers.size();loop1++)
    {
        if (allMultipleTrackers[loop1])
        {
            delete allMultipleTrackers[loop1];
            allMultipleTrackers[loop1] = NULL;
        }
    }
    if (pictureSaveThread)
    {
        pictureSaveThread->wait();
        delete pictureSaveThread;
        pictureSaveThread = NULL;
    }
    std::cout << "~VehicleConverseDetection()" << std::endl;
}

//车辆逆行检测
int VehicleConverseDetection::detect(const cv::Mat& frame)
{
    cv::Mat roiArea;//区域截图
    int objectNumber=0;
    std::vector<cv::Point2f> objectCenter;//检测的目标中心
    currentDate = QDateTime::currentDateTime().toString("yyyy:MM:dd-hh:mm:ss");
    int isConverse=0;
    errorCode=0;
    if (isFirstRun)
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
                objectNumber=0;
                roiArea = frame(detectArea[loop].getPolygonRect());
                objectCenter.clear();
                objectCenter=detectObjectCenter(roiArea, loop);
                tracking(roiArea,objectCenter,loop);
                calculateObjectConverse(loop);
                isConverse = converseArea(loop);
                objectNumber+=(int)objectCenter.size();
                if(errorCode<0)
                {
                    return errorCode;
                }
                else
                {
                    return isConverse;
               }
            }
        }
        //绘制检测区域
        //drawingDetectArea(frame,cv::Scalar(255,0,0));
        //绘制当前时间
        //cv::putText(frame, currentDate.toStdString().c_str(), cv::Point(15, 15),cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
    }
    return 0;

}

//每个跟踪区域重新开始跟踪
void VehicleConverseDetection::startTrcaking(bool isStart)
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
void VehicleConverseDetection::initDetectData()
{
    isFirstRun=true;
    errorCode=0;
    //加载参数
    loadConfig();
    frameForeground->initData();
    objectRecognition->initData(saveClassiferPath.toStdString());
    initData();
}

//匹配区域中是否有逆行目标
void VehicleConverseDetection::calculateObjectConverse(int number)
{
    std::vector<KalmanTracker*> trackers = allMultipleTrackers[number]->getListTrackers();
    int count = allMultipleTrackers[number]->getTrackersCount();
    std::cout << "tracker number:"<<count << std::endl;
    int loop=0;
    for (loop=0;loop<count;loop++)
    {
        if(isTrcakConverse(trackers[loop]->getTrace() ,detectArea[number].getNormalDirection()))
        {
            detectArea[number].isConverse=true;
            break;
        }
    }
    if (loop >= count)
    {
        detectArea[number].isConverse=false;
        detectArea[number].isFirst=0;
    }
}

//检测运动目标得到运动目标中心
std::vector<cv::Point2f> VehicleConverseDetection::detectObjectCenter(const cv::Mat &frame, int number)
{
    std::vector<cv::Point2f> myCenterPoint;
    std::vector<cv::Point2f> objectCenters;//检测目标的中心
    objectCenters.clear();
    myCenterPoint.clear();
    //objectCenters=frameForeground->getFrameForegroundCentroid(frame,minBox);
    objectCenters = frameForeground->getFrameForegroundCenter(frame, minBox);
    //objectRecognition->getFrameCarObejctdCenter(frame, minSize, minBox);
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
void VehicleConverseDetection::tracking(const cv::Mat& roi, const std::vector<cv::Point2f>& centers, int number)
{
    if(allMultipleTrackers[number])
    {
        allMultipleTrackers[number]->mutilpleTracking(roi, centers);
    }
}

//判断某个区域是否逆行
int VehicleConverseDetection::converseArea(int number)
{
    if(detectArea[number].isConverse)
    {
        detectArea[number].isConverse = false;
        return number + 1;
    }
    else
    {
        return 0;
    }
}

//该区域是否逆行并保存图片
int VehicleConverseDetection::converseArea(const cv::Mat& inFrame, int number, QString fileDir, QString fileName)
{
    //cv::Mat detectROI;
    if(detectArea[number].isConverse)
    {
        if(detectArea[number].isFirst==0)
        {
            detectArea[number].firstTime=QDateTime::currentDateTime();
            detectArea[number].isFirst=1;
            //deectROI=inFrame(detectArea[loop].getPolygonRect());
            pictureSaveThread->wait();
            errorCode=pictureSaveThread->initData(fileDir,fileName,inFrame);
            pictureSaveThread->start();
        }
        else
        {
            int tempDeltaTime=detectArea[number].firstTime.secsTo(QDateTime::currentDateTime());
            qDebug() << "deltaTime:" << tempDeltaTime;
            if(tempDeltaTime > saveDeltaTime)
            {
                //deectROI=inFrame(detectArea[loop].getPolygonRect());
                pictureSaveThread->wait();
                errorCode=pictureSaveThread->initData(fileDir,fileName,inFrame);
                pictureSaveThread->start();
            }
        }

        detectArea[number].isConverse=false;
        return number+1;
    }
    else
    {
        return 0;
    }
}

//判断路径是否逆方向
bool VehicleConverseDetection::isTrcakConverse(const std::vector<cv::Point2f> &points,int myDirection)
{
    int count=(int)points.size();
    int converseNumber=0;
    bool isConverse=false;
    if(count>1)
    {
        switch (myDirection)
        {
        case 1://上
            for(int loop=1;loop<count;loop++)
            {
                if((points[loop].y-points[loop-1].y)>0)
                {
                    converseNumber++;
                }
            }
            if(converseNumber>=minConversePointNum)
            {
                isConverse=true;
            }
            break;
        case 2://下
            for(int loop=1;loop<count;loop++)
            {
                if((points[loop].y-points[loop-1].y)<0)
                {
                    converseNumber++;
                }
            }
            if(converseNumber>=minConversePointNum)
            {
                isConverse=true;
            }
            break;
        case 3://左
            for(int loop=1;loop<count;loop++)
            {
                if((points[loop].x-points[loop-1].x)>0)
                {
                    converseNumber++;
                }
            }
            if(converseNumber>=minConversePointNum)
            {
                isConverse=true;
            }
            break;
        case 4://右
            for(int loop=1;loop<count;loop++)
            {
                if((points[loop].x-points[loop-1].x)<0)
                {
                    converseNumber++;
                }
            }
            if(converseNumber>=minConversePointNum)
            {
                isConverse=true;
            }
            break;
        default:
            isConverse=false;
            break;
        }
    }
    return isConverse;
}

//绘制检测区域
void VehicleConverseDetection::drawingDetectArea(cv::Mat &inFrame ,cv::Scalar color)
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
int VehicleConverseDetection::getErrorCode()
{
    return errorCode;
}

//得到检测区域
std::vector<ConverseArea>&  VehicleConverseDetection::getDetectArea()
{
    return detectArea;
}

void VehicleConverseDetection::initData()
{
    detectArea.clear();
    for (int loop1=0;loop1<(int)allMultipleTrackers.size();loop1++)
    {
        if (allMultipleTrackers[loop1])
        {
            delete allMultipleTrackers[loop1];
            allMultipleTrackers[loop1] = NULL;
        }
    }
    allMultipleTrackers.clear();

    ConverseArea area;
    int number=(int)pointsArea.size();
    for (int loop2=0; loop2<number; loop2++)
    {
        area.setPolygon(pointsArea[loop2]);
        area.setNormalDirection(areaDirection[loop2]);
        area.isConverse=false;
        area.isFirst=0;
        detectArea.push_back(area);
        KalmanMultipleTracker* tarcker=new KalmanMultipleTracker();
        allMultipleTrackers.push_back(tarcker);
    }
}

//初始化
void VehicleConverseDetection::init()
{
    errorCode=0;
    isFirstRun=true;

    detectArea.clear();
    pointsArea.clear();
    areaDirection.clear();
    allMultipleTrackers.clear();

    imageProcess=new ImageProcess();//图像处理算法类
    pictureSaveThread=new PictureSaveThread();//图片保存类
    geometryCalculations=new GeometryCalculations();//几何运算类
    frameForeground=new FrameForeground();//前景检测类
    objectRecognition=new ObjectRecognition();//目标识别

    savePictureFileName="image.png";

    loadConfig();
}

void VehicleConverseDetection::saveConfig()
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
    fs.open("./config/VehicleConverseDetection.xml",cv::FileStorage::WRITE,"utf-8");

    cv::write(fs,"isDrawObject", isDrawObject);
    cv::write(fs, "saveDeltaTime", saveDeltaTime);
    cv::write(fs, "minConversePointNum", minConversePointNum);
    cv::write(fs, "crossMatchMaxValue", crossMatchMaxValue);
    cv::write(fs, "minSize", minSize);
    cv::write(fs, "minBox", minBox);
    cv::write(fs, "savePictureDir", savePictureDir.toStdString());
    cv::write(fs, "saveClassiferPath", saveClassiferPath.toStdString());
    cv::write(fs, "areaDirection", areaDirection);
    for(int loop=0;loop<(int)pointsArea.size();loop++)
    {
        QString tempName="pointsArea"+QString::number(loop);
        cv::write(fs,tempName.toStdString().c_str(),pointsArea[loop]);
    }
    fs.release();
}

void VehicleConverseDetection::loadConfig()
{
    cv::FileStorage fs;
    std::vector<cv::Point> tempVector;
    cv::String tempDataDir="./intrude";
    cv::String tempPath = "./cascade.xml";
    pointsArea.clear();
    areaDirection.clear();
    fs.open("./config/VehicleConverseDetection.xml", cv::FileStorage::READ, "utf-8");

    cv::read(fs["isDrawObject"],isDrawObject, false);
    cv::read(fs["saveDeltaTime"], saveDeltaTime, 3);
    cv::read(fs["minConversePointNum"], minConversePointNum, 10);
    cv::read(fs["crossMatchMaxValue"], crossMatchMaxValue, 0.1f);
    cv::read(fs["minSize"], minSize, 30);
    cv::read(fs["minBox"], minBox, 300.0f);
    cv::read(fs["savePictureDir"], tempDataDir, cv::String("./converse"));
    cv::read(fs["saveClassiferPath"], tempPath, cv::String("./classifer/cascade.xml"));
    savePictureDir = QString::fromStdString(tempDataDir);
    saveClassiferPath = QString::fromStdString(tempPath);
    qDebug() << savePictureDir << " " << saveClassiferPath;
    cv::read(fs["areaDirection"], areaDirection);

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
