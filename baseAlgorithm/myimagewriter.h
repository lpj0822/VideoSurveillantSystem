#ifndef MYIMAGEWRITER_H
#define MYIMAGEWRITER_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

class MyImageWriter
{
public:
    MyImageWriter();
    ~MyImageWriter();

    //基于opencv的图片保存png
    int saveImage(const cv::Mat& frame, const std::string &fileNamePath);

private:

};

#endif // MYIMAGEWRITER_H
