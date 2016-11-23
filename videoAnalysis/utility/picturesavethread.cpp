#include "picturesavethread.h"
#include <iostream>
#include <QDir>
#include <QDebug>

PictureSaveThread::PictureSaveThread()
{
    init();
    std::cout << "PictureSaveThread()" << std::endl;
}

PictureSaveThread::~PictureSaveThread()
{
    if(imageWriter)
    {
        delete imageWriter;
        imageWriter = NULL;
    }
    std::cout << "~PictureSaveThread()" << std::endl;
}

void PictureSaveThread::run()
{
    int errorCode=0;
    if(isType)
    {
        errorCode = saveImage(image, savePictureFileName);
    }
    else
    {
        errorCode = imageWriter->saveImage(frame, savePictureFileName.toStdString());
    }
    emit signalPictureSaveFinish(savePictureFileName, errorCode);
}

int PictureSaveThread::initData(const QString &fileNameDir, const QString &fileName, QImage image)
{
    QDir makeDir;
    this->savePictureFileName = fileName;
    this->image = image.copy();
    isType = true;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkpath(fileNameDir))
        {
            std::cout << "make dir fail!" << std::endl;
            return -11;
        }
    }
    return 0;
}

int PictureSaveThread::initData(const QString &fileNameDir, const QString &fileName, const cv::Mat& frame)
{
    QDir makeDir;
    this->savePictureFileName = fileName;
    this->frame = frame.clone();
    isType = false;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkpath(fileNameDir))
        {
            std::cout << "make dir fail! "<< std::endl;
            return -11;
        }
    }
    std::cout << "fileName:" << fileName.toStdString() << std::endl;
    return 0;
}

//保存图片
int PictureSaveThread::saveImage(QImage image,const QString& fileNamePath)
{
    if(image.isNull())
    {
        return 0;
    }
    if(image.save(fileNamePath))
    {
        return 0;
    }
    else
    {
        qDebug() << "image save fail";
        return -10;
    }
}

void PictureSaveThread::init()
{
    isType = true;
    imageWriter = new MyImageWriter();
    savePictureFileName = "temp/image.png";
}
