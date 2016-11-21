#ifndef VEHICLECONVERSESHOWVIDEOWINDOW_H
#define VEHICLECONVERSESHOWVIDEOWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include <QPolygon>
#include <QPen>
#include <QFileInfo>
#include <QDateTime>
#include "vehicleConverse/vehicleconversedetectthread.h"
#include "utility/videowritethread.h"
#include "utility/errorcodestring.h"

class VehicleConverseShowVideoWindow : public QWidget
{
    Q_OBJECT
public:
    explicit VehicleConverseShowVideoWindow(QWidget *parent = 0);
    ~VehicleConverseShowVideoWindow();

    int showVideo(const QString &path);//显示视频
    int closeShowVideo();//关闭视频

    void setConfigParameter(QList<QPolygonF> detectArea, QList<int> directions);//配置检测参数

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startVideo();//开始播放
    void stopVideo();//停止播放

    QImage getImage();//得到图片

    bool getIsOpenVideo();//是否打开视频

signals:
    void signalConverseMessage(int number, QString savePath);
    void signalVideoMessage(bool isOpenVideo);

public slots:
    void slotVideoImage(QImage image,bool isOpen);
    void slotMessage(QString message,int pos);
    void slotSaveVideo();

protected:
    void paintEvent(QPaintEvent *e);

private:

    QString videoPath;//路径

    QImage currentImage;//获取的帧转化为QImage

    bool isOpen;//是否打开摄像头
    VehicleConverseDetectThread *vehicleConverseDetectThread;//车辆逆行检测

    VideoWriteThread *videoWriteThread;//保存视频

    int detectNumber;//检测到的区域


    std::vector< std::vector<cv::Point> > area;//检测区域
    QList<QPolygonF> copyArea;//绘制检测区域

    std::vector<int> areaDirection;//每个区域的正常行驶方向

    QPen myPen;//绘图画笔

    bool isFirstConverse;

    ErrorCodeString errorCodeString;


private:

    void drawingArea(QPixmap &pixmap);//绘制区域

    void initData();
    void initConnect();

    void saveVehicleConverseConfig();
    void loadVehicleConverseConfig();
};

#endif // VEHICLECONVERSESHOWVIDEOWINDOW_H
