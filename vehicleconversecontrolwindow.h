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
#include "videoAnalysis/vehicleconverseshowvideowindow.h"

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
    void slotConverseMessage(int number, QString savePath);

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

    bool isOpen;//是否打开视频
    bool isStartDetection;//是否开始检测

    void initUI();
    void initData();
    void initConnect();
};

#endif // VEHICLECONVERSECONTROLWINDOW_H
