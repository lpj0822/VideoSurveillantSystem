#include "picturesavethread.h"
#include <iostream>
#include <QDir>
#include <QDebug>

PictureSaveThread::PictureSaveThread()
{
    init();
    std::cout<<"PictureSaveThread()"<<std::endl;
}

PictureSaveThread::~PictureSaveThread()
{
    if(imageProcess)
    {
        delete imageProcess;
        imageProcess=NULL;
    }
    std::cout<<"~PictureSaveThread()"<<std::endl;
}

void PictureSaveThread::run()
{
    int errorCode=0;
    if(isStart)
    {
        if(isType)
        {
            errorCode=savePicture(image,savePictureFileName);
        }
        else
        {
            errorCode=imageProcess->savePicture(frame,savePictureFileName.toStdString());
        }
    }
    isStart=false;
    emit signalPictureSaveFinish(savePictureFileName,errorCode);
}

int PictureSaveThread::initData(const QString &fileNameDir, const QString &fileName,QImage image)
{
    QDir makeDir;
    this->savePictureFileName=fileName;
    this->image=image;
    isType=true;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkdir(fileNameDir))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return -11;
        }
    }
    isStart=true;
    return 0;
}

int PictureSaveThread::initData(const QString &fileNameDir, const QString &fileName,cv::Mat frame)
{
    QDir makeDir;
    this->savePictureFileName=fileName;
    this->frame=frame;
    isType=false;
    if(!makeDir.exists(fileNameDir))
    {
        if(!makeDir.mkdir(fileNameDir))
        {
            std::cout<<"make dir fail!"<<std::endl;
            return -11;
        }
    }
    std::cout<<"fileName:"<<fileName.toStdString()<<std::endl;
    isStart=true;
    return 0;
}

//保存图片
int PictureSaveThread::savePicture(QImage image,const QString& fileNamePath)
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
        qDebug()<<"image save fail";
        return -10;
    }
}

void PictureSaveThread::init()
{
    isStart=false;
    isType=false;
    imageProcess=new ImageProcess();
    savePictureFileName="temp/image.png";
}
