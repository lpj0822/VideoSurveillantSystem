#ifndef VEHICLECOUNTINGSHOWVIDEOWINDOW_H
#define VEHICLECOUNTINGSHOWVIDEOWINDOW_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include <QPolygon>
#include <QLine>
#include <QPen>
#include <QFileInfo>
#include <QDateTime>
#include "vehicleCouting/vehiclecountingthread.h"
#include "vehicleCouting/medianline.h"
#include "utility/errorcodestring.h"

class VehicleCountingShowVideoWindow : public QWidget
{
    Q_OBJECT
public:
    explicit VehicleCountingShowVideoWindow(QWidget *parent = 0);
    ~VehicleCountingShowVideoWindow();

    int showVideo(const QString &path);//显示视频
    int closeShowVideo();//关闭视频

    void setConfigParameter(QList<QPolygonF> detectArea,QList<int> directions);//配置检测参数

    void startDetect();//开始检测
    void stopDetect();//停止检测

    void startVideo();//开始播放
    void stopVideo();//停止播放

    QImage getImage();//得到图片

    bool getIsOpenVideo();//是否打开视频

signals:
    void signalCountingMessage(int count);
    void signalVideoMessage(bool isOpenVideo);

public slots:
    void slotVideoImage(QImage image,bool isOpen);
    void slotMessage(QString message);

protected:
    void paintEvent(QPaintEvent *e);

private:

    QString videoPath;//路径

    QImage currentImage;//获取的帧转化为QImage
    QPen myPen;//绘图画笔

    bool isOpen;//是否打开摄像头
    VehicleCountingThread *vehicleCountingThread;//车流量统计
    MedianLine medianLine;//计算中位线

    QList<QPolygonF> copyArea;//绘制检测区域
    QList<QLine> copyLine;//中位线

    std::vector< std::vector<cv::Point> > area;//检测区域
    std::vector<int> areaDirection;//每个区域的正常行驶方向

    ErrorCodeString errorCodeString;


private:

    void drawingArea(QPixmap &pixmap);//绘制区域

    void initData();
    void initConnect();

    void saveVehicleCountingConfig();
    void loadVehicleCountingConfig();
};

#endif // VEHICLECOUNTINGSHOWVIDEOWINDOW_H
