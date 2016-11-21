#ifndef LEAVEDETECTCONFIGUREWINDOW_H
#define LEAVEDETECTCONFIGUREWINDOW_H
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include "drawingwindow.h"

class LeaveDetectConfigureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LeaveDetectConfigureWindow(QImage image, QWidget *parent = 0);
    ~LeaveDetectConfigureWindow();

    QList<QPolygonF> getDetectArea();//得到检测区域
    int getLeaveMaxTime();//最大离岗时间，超过该时间则判定为离岗


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

    QLabel *maxLeaveTimeLabel;//最大离岗时间
    QSpinBox *maxLeaveTimeBox;

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

#endif // LEAVEDETECTCONFIGUREWINDOW_H
