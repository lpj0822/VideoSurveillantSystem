#ifndef LEAVEDETECTTHREAD_H
#define LEAVEDETECTTHREAD_H

#include <QThread>
#include <QImage>
#include "leavedetection.h"
#include "../videoprocess.h"
#include "../errorcodestring.h"

class LeaveDetectThread : public QThread
{
    Q_OBJECT

public:
    LeaveDetectThread();
    ~LeaveDetectThread();

    int startThread(const QString &path);//开始运行线程
    void stopThread();//结束运行线程

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startShowVideo();//开始播放
    void stopShowVideo();//停止播放

    bool getIsOpenVideo();//是否打开视频

    void initData();//初始化话数据

signals:
    void signalMessage(QString message,int pos);
    void signalVideoMessage(QImage image,bool isOpenVideo);

protected:
    void run();

private:

    QImage frameToimage;//获取的帧转化为QImage

    VideoProcess *videoProcess;//视频处理
    LeaveDetection *leaveDetection;//离岗检测
    cv::Mat readFrame;//读取的视频帧
    bool isOpen;//是否打开摄像头
    bool isStartDetection;//是否开始检测

    bool isStart;//开始运行线程
    bool isReadVideo;//是否读取视频

    ErrorCodeString errorCodeString;//错误信息类

private:

    void init();

};

#endif // LEAVEDETECTTHREAD_H
