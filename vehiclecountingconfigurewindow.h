#ifndef VEHICLECOUNTINGCONFIGUREWINDOW_H
#define VEHICLECOUNIINGCONFIGUREWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include "drawingwindow.h"

class VehicleCountingConfigureWindow : public QDialog
{
    Q_OBJECT
public:
    explicit VehicleCountingConfigureWindow(QImage image, QWidget *parent = 0);
    ~VehicleCountingConfigureWindow();

    QList<QPolygonF> getDetectArea();//得到检测区域
    QList<int> getDirection();//车辆正常行驶方向


signals:

public slots:
    void slotOk();
    void slotRest();

protected:
    void changeEvent(QEvent *event);

private:

    QVBoxLayout *mainLayout;//主布局

    QPixmap pixmap;//绘制区域画面
    DrawingWindow *drawArea;//绘图区域

    QLabel *directionLabel;//正常行车方向
    QComboBox *directionBox;

    QPushButton *okButton;//确定按钮
    QPushButton *cancelButton;//取消按钮
    QPushButton *restButton;//重绘

    QList<QPolygonF> area;//检测区域
    int widthWindow;//窗口长和宽
    int heightWindow;

    void initUI();//初始化话UI
    void initData();//初始化数据
    void initConnect();//事件连接

};

#endif // VEHICLECONVERSECONFIGURATIONWINDOW_H
