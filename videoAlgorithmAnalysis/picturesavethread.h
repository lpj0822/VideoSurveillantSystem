#ifndef PICTURESAVETHREAD_H
#define PICTURESAVETHREAD_H

#include <QThread>
#include <QObject>
#include <QImage>
#include "baseAlgorithm/imageprocess.h"

class PictureSaveThread : public QThread
{
    Q_OBJECT

public:
    PictureSaveThread();
    ~PictureSaveThread();

    int initData(const QString &fileNameDir, const QString &fileName,QImage image);
    int initData(const QString &fileNameDir, const QString &fileName,cv::Mat frame);

    int savePicture(QImage image,const QString& fileNamePath);//保存图片

signals:
    void signalPictureSaveFinish(QString name,int code);

protected:
    void run();

private:
    bool isStart;
    bool isType;

    QImage image;
    cv::Mat frame;
    ImageProcess *imageProcess;

    //保存截图的路径
    QString savePictureFileName;

    void init();

};

#endif // PICTURESAVETHREAD_H
