#ifndef INTRUDESHOWVIDEOWINDOW_H
#define INTRUDESHOWVIDEOWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include <QPolygon>
#include <QPen>
#include <QFileInfo>
#include <QDateTime>
#include "intrudeDetect/intrudedetectthread.h"
#include "utility/videowritethread.h"
#include "utility/errorcodestring.h"

class IntrudeShowVideoWindow : public QWidget
{
    Q_OBJECT
public:
    explicit IntrudeShowVideoWindow(QWidget *parent = 0);
    ~IntrudeShowVideoWindow();

    int showVideo(const QString &path);//显示视频
    int closeShowVideo();//关闭视频

    void setConfigParameter(QList<QPolygonF> detectArea);//配置检测参数

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startVideo();//开始播放
    void stopVideo();//停止播放

    QImage getImage();//得到图片

    bool getIsOpenVideo();//是否打开视频

signals:
    void signalIntrudeMessage(int number, QString savePath);
    void signalVideoMessage(bool isOpenVideo);

public slots:
    void slotVideoImage(QImage image, bool isOpen);
    void slotMessage(QString message, int pos);
    void slotSaveVideo();

protected:
    void paintEvent(QPaintEvent *e);

private:

    QString videoPath;//路径

    QImage currentImage;//获取的帧转化为QImage

    bool isOpen;//是否打开摄像头
    IntrudeDetectThread *intrudeDetectThread;//入侵检测

    VideoWriteThread *videoWriteThread;//保存视频

    int detectNumber;//检测到的区域


    std::vector< std::vector<cv::Point> > area;//检测区域
    QList<QPolygonF> copyArea;//绘制检测区域

    QPen myPen;//绘图画笔

    bool isFirstIntrude;

    ErrorCodeString errorCodeString;


private:

    void drawingArea(QPixmap &pixmap);//绘制区域

    void initData();
    void initConnect();

    void saveIntrudesConfig();
    void loadIntrudesConfig();
};

#endif // INTRUDESHOWVIDEOWINDOW_H
