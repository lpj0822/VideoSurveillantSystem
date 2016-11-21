#ifndef VEHICLECOUNTINGTHREAD_H
#define VEHICLECOUNTINGTHREAD_H

#include "vehiclecouting.h"
#include "utility/detectthread.h"

class VehicleCountingThread : public DetectThread
{
    Q_OBJECT

public:
    VehicleCountingThread();
    ~VehicleCountingThread();

    void initData();//初始化话数据

signals:
    void signalMessage(QString message);

protected:
    void detect(const cv::Mat& frame);

private:   
    VehicleCouting *vehicleCouting;//车流量统计

private:
    void init();
};

#endif // VEHICLECOUNTINGTHREAD_H
