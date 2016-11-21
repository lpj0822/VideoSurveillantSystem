#ifndef VIDEOSAVE_H
#define VIDEOSAVE_H

#include <QObject>
#include "videosavethread.h"
#include "errorcodestring.h"

#define MAXVIDEOTHREAD 10

class VideoSave : public QObject
{
    Q_OBJECT
public:
    VideoSave(QObject *parent = 0);
    ~VideoSave();

    void saveVideoData(QString videoPath, QString savePath, QString saveFileName, int stopPos, int intervalSec);

signals:
    void signalSaveFinish(QString name);

public slots:

private:
    int numThread;
    VideoSaveThread* saveThread[MAXVIDEOTHREAD];

    ErrorCodeString errorCodeString;//错误信息类

private:
    void init();
};

#endif // VIDEOSAVE_H
