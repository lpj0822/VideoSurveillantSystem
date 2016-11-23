#ifndef CONVERSEAREA_H
#define CONVERSEAREA_H
#include "../utility/polygonarea.h"
#include <QDateTime>

class ConverseArea : public PolygonArea
{
public:
    ConverseArea();
    ~ConverseArea();

    void setNormalDirection(int direction);
    int getNormalDirection();

    bool isConverse;//是否逆行

private:
    int normalDirection;//车辆正常方向
    void initData();//初始化数据
};

#endif // CONVERSEAREA_H
