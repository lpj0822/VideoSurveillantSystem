#ifndef LEAVEDETECTTHREAD_H
#define LEAVEDETECTTHREAD_H

#include "leavedetection.h"
#include "utility/detectthread.h"

class LeaveDetectThread : public DetectThread
{
    Q_OBJECT

public:
    LeaveDetectThread();
    ~LeaveDetectThread();

    void initData();//初始化数据

signals:
    void signalMessage(QString message, int pos);

protected:
    void detect(const cv::Mat& frame);

private:
    LeaveDetection *leaveDetection;//离岗检测

private:
    void init();

};

#endif // LEAVEDETECTTHREAD_H
