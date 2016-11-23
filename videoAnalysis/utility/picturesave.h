#ifndef PICTURESAVE_H
#define PICTURESAVE_H

#include <QObject>
#include "picturesavethread.h"
#include "errorcodestring.h"

#define MAXPICTURETHREAD 10

class PictureSave : public QObject
{
    Q_OBJECT
public:
    PictureSave(QObject *parent = 0);
    ~PictureSave();

    void savePictureData(QString savePath ,QString saveFileName, QImage image);

    void savePictureData(QString savePath, QString saveFileName, const cv::Mat& frame);

signals:
    void signalSaveFinish(QString name, int code);

public slots:

private:
    int numThread;
    PictureSaveThread* saveThread[MAXPICTURETHREAD];

    ErrorCodeString errorCodeString;//错误信息类

private:
    void init();
};

#endif // PICTURESAVE_H
