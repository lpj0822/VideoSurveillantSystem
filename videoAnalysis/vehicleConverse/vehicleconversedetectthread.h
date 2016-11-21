#ifndef VEHICLECONVERSEDETECTTHREAD_H
#define VEHICLECONVERSEDETECTTHREAD_H

#include <QDateTime>
#include "vehicleconversedetection.h"
#include "utility/detectthread.h"

class VehicleConverseDetectThread : public DetectThread
{
    Q_OBJECT

public:
    VehicleConverseDetectThread();
    ~VehicleConverseDetectThread();

    void initData();//初始化数据

signals:
    void signalMessage(QString message, int pos);
    void signalSaveVideo();

protected:
    void detect(const cv::Mat& frame);

private:
    VehicleConverseDetection *vehicleConverseDetection;//车辆逆行检测

    bool isConverse;
    QDateTime saveTime;
    int delayTime;//两次逆行最短时间

private:
    void init();

};

#endif // VEHICLECONVERSEDETECTTHREAD_H
