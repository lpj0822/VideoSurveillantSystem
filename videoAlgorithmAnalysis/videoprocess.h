#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H
//opencv
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

//Qt
#include <QObject>

class VideoProcess : public QObject
{
    Q_OBJECT
public:
    VideoProcess(QObject *parent = 0);
    ~VideoProcess();

    int openVideo(int videoNumber=0,int width=640,int height=480);//打开摄像头
    int openVideo(const QString& fileName);//打开视频文件
    int closeVideo();//关闭视频
    int readFrame(cv::Mat &frame);//读取视频帧
    int setFramePosition(int position=0);//设置视频帧的位置
    int getFramePosition();//获得当前帧位置
    int getFramePositionmSec();//得到当前视频的位置ms
    double getFrameFPS();//获得视频帧率
    int getFrameCount();//视频的总帧数
    int getVideoFOURCC();//返回视频编码
    cv::Size getSize();//返回视频大小
    bool isOpen();//是否打开视频

    int getErrorCode();//得到错误码

public slots:

private:
    cv::VideoCapture capture;//摄像头
    int videoNumber;
    bool isOpenVideo;

    int captureWidth;//采集视频画面大小
    int captureHeight;

    int errorCode;//错误码

    void initData();//初始化数据

private:

    void saveConfig();
    void loadConfig();
};

#endif // VIDEOPROCESS_H
