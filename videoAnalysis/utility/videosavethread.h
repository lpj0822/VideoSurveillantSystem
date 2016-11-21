#ifndef VIDEOSAVETHREAD_H
#define VIDEOSAVETHREAD_H

#include <QThread>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include "videoprocess.h"

class VideoSaveThread : public QThread
{ 
    Q_OBJECT

public:
    VideoSaveThread();
    ~VideoSaveThread();

    int openVideo(const QString& fileName);//打开视频文件

    int initSaveVideoData(const QString &fileNameDir, const QString &fileName, long startPos, long intervalSec, bool inv=false);//初始化保存数据参数

    int initSaveVideoData1(const QString &fileNameDir, const QString &fileName, int startSec, int intervalSec, bool inv=false);//初始化保存数据参数

    void startThread();//开始线程
    void stopThread();//结束线程

    void closeVideoWrite();//关闭视频写入
    void closeVideo();//关闭视频

signals:
    void signalVideoSaveFinish(QString name);

public slots:

protected:
    void run();

private:
    VideoProcess *video;
    cv::VideoWriter outputVideo;//保存视频

    QString fileName;//保存的文件名

    bool isStart;//是否允许线程

    cv::Size size;//保存视频文件的大小
    double fps;//帧率
    int codec;//视频编码格式
    bool isColor;//是否是彩色

    long startFrame;//起始帧
    long stopFrame;//结束帧

private:
    void init();

    int saveVideo(cv::Mat inFrame);//保存视频文件
};

#endif // VIDEOSAVETHREAD_H
