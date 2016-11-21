#ifndef LEAVESHOWVIDEOWINDOW_H
#define LEAVESHOWVIDEOWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include <QPolygon>
#include <QPen>
#include <QFileInfo>
#include "leaveDetect/leavedetectthread.h"
#include "utility/videosave.h"

class LeaveShowVideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LeaveShowVideoWindow(QWidget *parent = 0);
    ~LeaveShowVideoWindow();

    int showVideo(const QString &path);//显示视频
    int closeShowVideo();//关闭视频

    void setConfigParameter(QList<QPolygonF> detectArea,int maxLeaveTime=300);//配置检测参数

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startVideo();//开始播放
    void stopVideo();//停止播放

    QImage getImage();//得到图片

    bool getIsOpenVideo();//是否打开视频

signals:
    void signalLeaveMessage(int number);
    void signalSaveVideoMessage(QString path);
    void signalVideoMessage(bool isOpenVideo);

public slots:
    void slotVideoImage(QImage image,bool isOpen);
    void slotMessage(QString message,int pos);
    void slotSaveVideo(QString path);

protected:
    void paintEvent(QPaintEvent *e);

private:

    QString videoPath;//路径

    QImage currentImage;//获取的帧转化为QImage

    bool isOpen;//是否打开摄像头
    LeaveDetectThread *leaveDetectThread;//离岗检测
    VideoSave *videoSave;//保存视频

    int maxLeaveTime;//最大离岗时间，超过这个时间就报警s

    std::vector< std::vector<cv::Point> > area;//办公区域
    QList<QPolygonF> copyArea;//办公区域

    QPen myPen;//绘图画笔

private:

    void drawingArea(QPixmap &pixmap);//绘制区域

    void initData();
    void initConnect();

    void saveLeaveConfig();
    void loadLeaveConfig();
};

#endif // LEAVESHOWVIDEOWINDOW_H
