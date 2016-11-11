#ifndef COUNTINGAREA_H
#define COUNTINGAREA_H
#include "../utility/polygonarea.h"

class CountingArea : public PolygonArea
{
public:
    CountingArea();
    ~CountingArea();

    void setNormalDirection(int direction);
    int getNormalDirection();

    void setVehicleCouting(int count);
    void addVehicleCouting(int count=1);
    int getVehicleCouting();

    void setMedianPoint(int position);
    int getMedianPoint();

private:
    int moveMedianPoint;//矩形中点
    int vehicleCouting;//车流量
    int normalDirection;//车辆正常方向
    void init();//初始化数据
};

#endif // COUNTINGAREA_H
