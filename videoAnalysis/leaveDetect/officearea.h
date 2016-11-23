#ifndef OFFICEAREA_H
#define OFFICEAREA_H

#include <QDateTime>
#include "../utility/polygonarea.h"

class OfficeArea: public PolygonArea
{
public:
    OfficeArea();
    ~OfficeArea();

    void setLeaveTime(int initTime);
    int getLeaveTime();
    void addLeaveTime(int leaveTime);//增加离开时间

    void setLeaveCount(int count);
    int getLeaveCount();
    void addLeaveCount(int count);//增加离岗次数

    QDateTime firstLeaveTime;//第一次离岗的时间
    int isFirstLeave;//第一次检测到离岗

private:

    int leaveTime;//离岗时间
    int leaveCount;//离岗次数

    void initData();//初始化数据
};

#endif // OFFICEAREA_H
