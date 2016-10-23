#ifndef VEHICLECONVERSECONTROLWINDOW_H
#define VEHICLECONVERSECONTROLWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QTextBrowser>
#include <QVector>
#include <QPoint>

#include "videoAlgorithmAnalysis/vehicleconverseshowvideowindow.h"

class VehicleConverseControlWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VehicleConverseControlWindow(QWidget *parent = 0);
    ~VehicleConverseControlWindow();

    void drawingArea(QPixmap &pixmap);//绘制区域

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void slotOpenVideo();
    void slotStartDetection();
    void slotConfiguration();
    void slotClearText();

    void slotVideoMessage(bool isVideoOpen);
    void slotConverseMessage(int number,QString savePath);

private:

    QPushButton *openVideoButton;
    QPushButton *startDetectionButton;
    QPushButton *configurationButton;
    QPushButton *clearTextButton;

    QTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    VehicleConverseShowVideoWindow *lbPlay;//车辆逆行检测

    QString path;//视频路径

    QList<QPolygonF> area;//检测区域
    QList<int> areaDirection;//每个区域的正常行驶方向

    int minConversePointNum;//运动路径中至少多少个逆行点

    int algorithm;//算法类型
    bool isFilter;//是否滤波
    int filterType;//滤波器的类型

    int minObjectArea;//最小能检测目标的面积
    float areaCrossValue;//区域相交度最小值

    double distThreshold;//两帧之间目标最大的移动距离
    int maxAllowedSkippedFrames;//允许目标消失的最大帧数
    int maxTraceLength;//跟踪轨迹的最大长度

    bool isOpen;//是否打开视频
    bool isStartDetection;//是否开始检测

    void initUI();
    void initData();
    void initConnect();

    void saveVehicleConverseConfig();
    void loadVehicleConverseConfig();
    void saveForegroundConfig();
    void loadForegroundConfig();
    void loadMultipleTrackerConfig();
    void saveMultipleTrackerConfig();
};

#endif // VEHICLECONVERSECONTROLWINDOW_H
