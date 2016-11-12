#ifndef VEHICLECOUTING_H
#define VEHICLECOUTING_H

#include <QObject>
#include "baseAlgorithm/frameforeground.h"
#include "multipletracking/blobmultipletracker.h"
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

    void tracking(const cv::Mat& roi,int number);//对目标进行多目标跟踪
    void countingVehicle(int number);//统计车流量

    void drawingDetectArea(cv::Mat &inFrame , cv::Scalar color=cv::Scalar(255,255,255));//绘制检测区域

    void initData();

private:

    MedianLine medianLine;//中间位置
    FrameForeground *frameForeground;//前景检测类
    std::vector<BlobMultipleTracker *> allTracker;//多目标跟踪类

    std::vector<CountingArea> detectArea;//整个检测区域
    std::vector< std::vector<cv::Point> > pointsArea;
    std::vector<int> areaDirection;//每个区域的正常行驶方向

    int errorCode;//错误码
    bool isFirstRun;//第一次运行

private:
    void init();//初始化数据
    void saveConfig();
    void loadConfig();
};

#endif // VEHICLECOUTING_H
