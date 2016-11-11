#ifndef MYVIDEOWRITER_H
#define MYVIDEOWRITER_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


class MyVideoWriter
{
public:
    MyVideoWriter();
    ~MyVideoWriter();

    int initSaveVideoData(const std::string &fileName, cv::Size size, double fps=25,
                          int codec=cv::VideoWriter::fourcc('X','V','I','D'), bool isColor=true);//初始化保存数据参数
    void closeWriteVideo();//关闭视频写入
    int saveVideo(const cv::Mat& inFrame);//保存视频文件
    int getWriteFrameBytes();//得到写入视频的帧字节数

private:   
    cv::VideoWriter outputVideo;//保存视频

    void init();
};

#endif // MYVIDEOWRITER_H
