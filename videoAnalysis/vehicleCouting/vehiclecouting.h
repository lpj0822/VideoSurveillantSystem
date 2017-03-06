/*
 * Vehicle counting
 * 车流量统计
 */
#ifndef VEHICLECOUTING_H
#define VEHICLECOUTING_H

#include <QObject>
#include "baseAlgorithm/imageprocess.h"
#include "baseAlgorithm/frameforeground.h"
#include "baseAlgorithm/geometrycalculations.h"
#include "multipletracking/kalmanmultipletracker.h"
#include "countingarea.h"
#include "medianline.h"

class VehicleCouting : public QObject
{
    Q_OBJECT
public:
    VehicleCouting(QObject *parent = 0);
    ~VehicleCouting();

    void initDetectData();//初始化检测参数
    int detect(const cv::Mat &frame);//车流量统计
    void startTrcaking(bool isStart);//每个跟踪区域重新开始跟踪

    int getErrorCode();//得到错误码
    std::vector<CountingArea>&  getDetectArea();//得到检测区域

    QString currentDate;//当前时间

signals:
    void signalMessage(QString message);

private:

    std::vector<cv::Point2f> detectObjectCenter(const cv::Mat& frame, int number);//检测运动目标得到检测区域内的目标中心点
    void tracking(const cv::Mat& roi, const std::vector<cv::Point2f>& centers, int number);//对目标进行多目标跟踪
    void countingVehicle(int number);//统计车流量

    void initData();

private:

    MedianLine medianLine;//中间位置
    ImageProcess *imageProcess;//图像处理算法类
    GeometryCalculations *geometryCalculations;//几何运算类
    FrameForeground *frameForeground;//前景检测类
    std::vector<KalmanMultipleTracker *> allMultipleTrackers;//多目标跟踪类

    std::vector<CountingArea> detectArea;//整个检测区域
    std::vector< std::vector<cv::Point> > pointsArea;
    std::vector<int> areaDirection;//每个区域的正常行驶方向

    float minBox;//检测的最小目标面积大小
    bool isDrawObject;

    int errorCode;//错误码
    bool isFirstRun;//第一次运行

private:
    void init();//初始化数据
    void saveConfig();
    void loadConfig();
};

#endif // VEHICLECOUTING_H
