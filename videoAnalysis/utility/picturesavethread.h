#ifndef PICTURESAVETHREAD_H
#define PICTURESAVETHREAD_H

#include <QThread>
#include <QObject>
#include <QImage>
#include "baseAlgorithm/myimagewriter.h"

class PictureSaveThread : public QThread
{
    Q_OBJECT

public:
    PictureSaveThread();
    ~PictureSaveThread();

    int initData(const QString &fileNameDir, const QString &fileName,QImage image);
    int initData(const QString &fileNameDir, const QString &fileName, const cv::Mat& frame);

signals:
    void signalPictureSaveFinish(QString name, int code);

protected:
    void run();

private:
    bool isType;

    QImage image;
    cv::Mat frame;
    MyImageWriter *imageWriter;

    //保存截图的路径
    QString savePictureFileName;

private:
    int saveImage(QImage image, const QString& fileNamePath);//保存图片
    void init();

};

#endif // PICTURESAVETHREAD_H
