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
#include "videoAnalysis/vehiclecountingshowvideowindow.h"

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

    bool isOpen;//是否打开视频
    bool isStartDetection;//是否开始检测

    void initUI();
    void initData();
    void initConnect();
};

#endif // VEHICLECOUNTINGCONTROLWINDOW_H
