#ifndef VEHICLECONVERSEDETECTTHREAD_H
#define VEHICLECONVERSEDETECTTHREAD_H

#include <QThread>
#include <QImage>
#include <QDateTime>
#include "vehicleconversedetection.h"
#include "../videoprocess.h"
#include "../errorcodestring.h"

class VehicleConverseDetectThread : public QThread
{
    Q_OBJECT

public:
    VehicleConverseDetectThread();
    ~VehicleConverseDetectThread();

    int startThread(const QString &path);//开始运行线程
    void stopThread();//结束运行线程

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startShowVideo();//开始播放
    void stopShowVideo();//停止播放

    bool getIsOpenVideo();//是否打开视频

    void initData();//初始化数据

signals:
    void signalMessage(QString message,int pos);
    void signalVideoMessage(QImage image,bool isOpenVideo);
    void signalSaveVideo();

protected:
    void run();

private:

    QImage frameToimage;//获取的帧转化为QImage

    VideoProcess *videoProcess;//视频处理
    VehicleConverseDetection *vehicleConverseDetection;//车辆逆行检测
    cv::Mat readFrame;//读取的视频帧
    bool isOpen;//是否打开摄像头
    bool isStartDetection;//是否开始检测

    bool isStart;//开始运行线程
    bool isReadVideo;//是否读取视频

    ErrorCodeString errorCodeString;//错误信息类

    bool isConverse;
    QDateTime saveTime;

    int delayTime;//两次逆行最短时间

private:

    void init();

};

#endif // VEHICLECONVERSEDETECTTHREAD_H
