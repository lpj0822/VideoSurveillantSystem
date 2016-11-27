/*
 *Leave detection
 * 离岗检测
 */
#ifndef LEAVEDETECTION_H
#define LEAVEDETECTION_H

#include <QObject>
#include "baseAlgorithm/imageprocess.h"
#include "baseAlgorithm/geometrycalculations.h"
#include "baseAlgorithm/frameforeground.h"
#include "../utility/picturesavethread.h"
#include "../utility/errorcodestring.h"
#include "officearea.h"

class LeaveDetection : public QObject
{
    Q_OBJECT
public:
    LeaveDetection(QObject *parent = 0);
    ~LeaveDetection();

    void initDetectData();//初始化检测参数
    int detect(const cv::Mat &frame);//离岗检测

    int getErrorCode();//得到错误码
    std::vector<OfficeArea>  getOfficeArea();//得到办公区域

    QString currentDate;//当前时间

signals:

public slots:

private:
    std::vector<cv::Rect> detectObject(const cv::Mat& frame);//检测运动目标
    void matchOfficeArea(std::vector<cv::Rect> vectorRect,int number);//对某一个区域匹配目标
    int leaveArea(int number);//判断某个区域是否离岗


private:
    ImageProcess *imageProcess;//图像处理算法类
    GeometryCalculations *geometryCalculations;//几何运算类
    FrameForeground *frameForeground;//前景检测类

    std::vector<OfficeArea> office;//整个办公区域
    std::vector< std::vector<cv::Point> > pointsArea;

    int maxLeaveCount;//最大离岗次数
    int maxLeaveTime;//离岗最长时间s
    float crossMatchMaxValue;//两矩形相交的临界值，大于这个值就认为匹配
    float minBox;//检测的最小目标面积大小
    bool isDrawObject;

    ErrorCodeString errorCodeString;

    int errorCode;//错误码
    bool isFirstRun;//第一次运行


    void initData();//初始化数据

private:
    void saveConfig();
    void loadConfig();

};

#endif // LEAVEDETECTION_H
