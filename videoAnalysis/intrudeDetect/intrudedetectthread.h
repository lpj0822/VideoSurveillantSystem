#ifndef INTRUDEDETECTTHREAD_H
#define INTRUDEDETECTTHREAD_H

#include <QThread>
#include <QImage>
#include <QDateTime>
#include "intrusiondetection.h"
#include "../videoprocess.h"
#include "../errorcodestring.h"

class IntrudeDetectThread : public QThread
{
    Q_OBJECT

public:
    IntrudeDetectThread();
    ~IntrudeDetectThread();

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
    void signalSaveVideo();

protected:
    void run();

private:

    QImage frameToimage;//获取的帧转化为QImage

    VideoProcess *videoProcess;//视频处理
    IntrusionDetection *intrudeDetection;//入侵检测
    cv::Mat readFrame;//读取的视频帧
    bool isOpen;//是否打开摄像头
    bool isStartDetection;//是否开始检测

    bool isStart;//开始运行线程
    bool isReadVideo;//是否读取视频

    ErrorCodeString errorCodeString;//错误信息类

    bool isIntrude;
    QDateTime saveTime;

    int delayTime;//两次区域入侵最短时间

private:

    void init();

};

#endif // INTRUDEDETECTTHREAD_H
