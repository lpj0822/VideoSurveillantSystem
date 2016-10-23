#ifndef INTRUSIONAREA_H
#define INTRUSIONAREA_H

#include "../baseAlgorithm/polygonarea.h"
#include <QDateTime>

class IntrusionArea : public PolygonArea
{
public:
    IntrusionArea();
    ~IntrusionArea();

    void setIntrusionCount(int count);
    void addIntrusionCount(int count);
    int getIntrusionCount();

    bool isIntrusion;//是否入侵
    int isFirst;//第一次检测到目标
    QDateTime firstTime;//第一次检测到目标的时间

private:
    int intrusionCount;//入侵次数

    void initData();//初始化数据

};

#endif // INTRUSIONAREA_H
