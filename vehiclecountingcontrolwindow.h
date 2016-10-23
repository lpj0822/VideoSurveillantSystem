#ifndef VEHICLECOUNTINGCONTROLWINDOW_H
#define VEHICLECOUNTINGCONTROLWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QTextBrowser>
#include <QVector>
#include <QPoint>
#include "videoAlgorithmAnalysis/vehiclecountingshowvideowindow.h"

class VehicleCountingControlWindow : public QWidget
{
    Q_OBJECT

public:
    VehicleCountingControlWindow(QWidget *parent = 0);
    ~VehicleCountingControlWindow();

    void drawingArea(QPixmap &pixmap);//绘制区域

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void slotOpenVideo();
    void slotStartDetection();
    void slotConfiguration();
    void slotClearText();

    void slotVideoMessage(bool isVideoOpen);
    void slotCountingMessage(int count);

private:

    QPushButton *openVideoButton;
    QPushButton *startDetectionButton;
    QPushButton *configurationButton;
    QPushButton *clearTextButton;

    QLabel *vehicleCountLabel;//显示车流量

    QTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    VehicleCountingShowVideoWindow *lbPlay;//车流量统计

    QString path;//视频路径

    QList<QPolygonF> area;//检测区域
    QList<int> areaDirection;//每个区域的正常行驶方向

    int algorithm;//算法类型
    bool isFilter;//是否滤波
    int filterType;//滤波器的类型

    int minArea;//最小目标的面积
    double distThreshold;//两帧之间目标最大的移动距离
    int maxAllowedSkippedFrames;//允许目标消失的最大帧数
    int maxTraceLength;//跟踪轨迹的最大长度

    bool isOpen;//是否打开视频
    bool isStartDetection;//是否开始检测

    void initUI();
    void initData();
    void initConnect();

    void saveVehicleCountingConfig();
    void loadVehicleCountingConfig();
    void saveForegroundConfig();
    void loadForegroundConfig();
    void loadMultipleTrackerConfig();
    void saveMultipleTrackerConfig();
};

#endif // VEHICLECOUNTINGCONTROLWINDOW_H
