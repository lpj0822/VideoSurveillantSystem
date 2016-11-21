#ifndef INTRUDEDETECTTHREAD_H
#define INTRUDEDETECTTHREAD_H

#include <QDateTime>
#include "intrusiondetection.h"
#include "utility/detectthread.h"

class IntrudeDetectThread : public DetectThread
{
    Q_OBJECT

public:
    IntrudeDetectThread();
    ~IntrudeDetectThread();

    void initData();//初始化话数据

signals:
    void signalMessage(QString message, int pos);
    void signalSaveVideo();

protected:
    void detect(const cv::Mat& frame);

private:
    IntrusionDetection *intrudeDetection;//入侵检测

    bool isIntrude;
    QDateTime saveTime;
    int delayTime;//两次区域入侵最短时间

private:
    void init();

};

#endif // INTRUDEDETECTTHREAD_H
