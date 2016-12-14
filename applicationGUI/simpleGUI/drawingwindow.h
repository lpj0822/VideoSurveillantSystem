#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <iostream>

#include <QWidget>
#include <QPixmap>
#include <QPen>
#include <QVector>
#include <QPoint>
#include <QLabel>

class DrawingWindow : public QWidget
{
    Q_OBJECT
public:
    DrawingWindow(QPixmap image,QWidget *parent = 0);
    ~DrawingWindow();

    double distancePoint(QPoint p1,QPoint p2);//求两点之间的距离

    void restImage(QPixmap pixmap);//重绘

    QVector<QPoint> getPolygon();


signals:

public slots:

protected:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent * e);
    void mousePressEvent(QMouseEvent * e);

private:

    void initUI();
    void initData();

    bool aviable_complete_polygon;//多边形是否完成
    bool isStart;//第一个坐标点是否存在

    QPixmap image;

    QPoint startPoint;//起始点
    QPoint prePoint;//前一个点
    QPoint tempPoint;//移动中的点
    QPen myPen;//绘图画笔
    QVector<QPoint> polygonPoints;//存储多边形点
};

#endif // DRAWINGWINDOW_H
