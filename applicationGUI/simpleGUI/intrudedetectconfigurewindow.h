#ifndef INTRUDEDETECTCONFIGUREWINDOW_H
#define INTRUDEDETECTCONFIGUREWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPolygonF>
#include "drawingwindow.h"

class IntrudeDetectConfigureWindow : public QDialog
{
    Q_OBJECT
public:
    explicit IntrudeDetectConfigureWindow(QImage image, QWidget *parent = 0);
    ~IntrudeDetectConfigureWindow();

    QList<QPolygonF> getDetectArea();//得到检测区域

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

#endif // INTRUDEDETECTCONFIGUREWINDOW_H
