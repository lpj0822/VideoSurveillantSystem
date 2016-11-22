#include "myimagewriter.h"
#include <iostream>

MyImageWriter::MyImageWriter()
{
    std::cout << "MyImageWriter()" << std::endl;
}

MyImageWriter::~MyImageWriter()
{
    std::cout << "~MyImageWriter()" << std::endl;
}

//基于opencv的图片保存png
int MyImageWriter::saveImage(const cv::Mat &frame, const std::string &fileNamePath)
{
    if (frame.empty())
    {
        return 0;
    }
    std::vector<int> params;//参数
    std::cout << "fileName:" << fileNamePath << std::endl;
    params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    params.push_back(6);//PNG格式的压缩级别

    if(!cv::imwrite(fileNamePath, frame, params))//保存图片
    {
        std::cout<<"imwrite fail!"<<std::endl;
        return -10;
    }
    return 0;
}
