#include "detectedpicturesave.h"
#include <iostream>
#include <QDebug>

DetectedPictureSave::DetectedPictureSave(QObject *parent) : QObject(parent)
{
    init();
    initConnect();
    std::cout << "DetectedPictureSave()" << std::endl;
}

DetectedPictureSave::~DetectedPictureSave()
{
    std::cout << "~DetectedPictureSave()" << std::endl;
}

//该区域是否入侵并保存图片
void DetectedPictureSave::saveDetectedResultToPicture(QString fileDir, QString fileName, const cv::Mat& frame)
{
    if(isFirst)
    {
        saveTime = QDateTime::currentDateTime();
        isFirst= false;
        pictureSave->savePictureData(fileDir, fileName, frame);
    }
    else
    {
        int tempDeltaTime = saveTime.secsTo(QDateTime::currentDateTime());
        qDebug() << "deltaTime:" << tempDeltaTime;
        if(tempDeltaTime >= deltaTime)
        {
            pictureSave->savePictureData(fileDir, fileName, frame);
            saveTime = QDateTime::currentDateTime();
        }
    }

}

void DetectedPictureSave::init()
{
    pictureSave = new PictureSave();//图片保存类
    isFirst = true;//第一次检测到结果
    deltaTime = 3;//间隔多长时间保存一张图片s
}

void DetectedPictureSave::initConnect()
{
    connect(pictureSave, &PictureSave::signalSaveFinish, this, &DetectedPictureSave::signalSaveFinish);
}
