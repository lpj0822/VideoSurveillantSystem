#ifndef VIDEOWRITETHREAD_H
#define VIDEOWRITETHREAD_H

#include <QThread>
#include "baseAlgorithm/myvideowriter.h"
#include "videoprocess.h"

class VideoWriteThread : public QThread
{
    Q_OBJECT

public:
    VideoWriteThread();
    ~VideoWriteThread();

    int openVideo(const QString& fileName);//打开视频文件

    int initSaveVideoData(const QString &fileNameDir, const QString &fileName);//初始化保存数据参数

    void setStartPos(int pos);//设置保存开始帧
    void setStopPos(int pos);//设置保存结束帧

    void startThread();//开始线程
    void stopThread();//结束线程

    void closeVideoWrite();//关闭视频写入
    void closeVideo();//关闭视频

    QString getSaveFileName();//得到保存路径

    int getFrameByets();//得到写入视频帧的字节数

signals:

public slots:

protected:
    void run();

private:
    VideoProcess *video;
    MyVideoWriter *outputVideo;//保存视频

    QString fileName;//保存的文件名

    bool isStart;//是否允许线程

    cv::Size size;//保存视频文件的大小
    double fps;//帧率
    int codec;//视频编码格式
    bool isColor;//是否是彩色

    int startFrame;//起始帧
    int stopFrame;//结束帧

private:
    void init();

};

#endif // VIDEOWRITETHREAD_H
