#ifndef DETECTTHREAD_H
#define DETECTTHREAD_H

#include <QThread>
#include <QImage>
#include <opencv2/imgproc.hpp>
#include "videoprocess.h"
#include "errorcodestring.h"

class DetectThread: public QThread
{
    Q_OBJECT

public:
    DetectThread();
    virtual ~DetectThread();

    int startThread(const QString &path);//开始运行线程
    void stopThread();//结束运行线程

    void startShowVideo();//开始播放
    void stopShowVideo();//停止播放

    void startDetect();//开始检测
    void stopDetect();//停止检测

    bool getIsOpenVideo();//是否打开视频

signals:
    void signalVideoMessage(QImage image, bool isOpenVideo);

protected:
    void run();
    virtual void detect(const cv::Mat& frame) = 0;

protected:
    VideoProcess *videoProcess;//视频处理
    ErrorCodeString errorCodeString;//错误信息类

private:

    QImage frameToimage;//获取的帧转化为QImage
    cv::Mat readFrame;//读取的视频帧
    bool isOpen;//是否打开摄像头
    bool isStartDetection;//是否开始检测

    bool isStart;//开始运行线程
    bool isReadVideo;//是否读取视频

private:
    void init();

};

#endif // DETECTTHREAD_H
