#ifndef DETECTEDPICTURESAVE_H
#define DETECTEDPICTURESAVE_H

#include <QObject>
#include <QDateTime>
#include "picturesave.h"

class DetectedPictureSave : public QObject
{
    Q_OBJECT
public:
    DetectedPictureSave(QObject *parent = 0);
    ~DetectedPictureSave();

    void saveDetectedResultToPicture(QString fileDir, QString fileName, const cv::Mat& frame);

signals:
    void signalSaveFinish(QString name, int code);

public slots:

private:
    PictureSave *pictureSave;//图片保存类
    bool isFirst;//第一次检测到结果
    QDateTime saveTime;//第一次检测到结果的时间
    int deltaTime;//间隔多长时间保存一张图片s

    void init();
    void initConnect();
};

#endif // DETECTEDPICTURESAVE_H
