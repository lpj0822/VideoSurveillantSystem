#include "drawingwindow.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>
#include <iostream>

DrawingWindow::DrawingWindow(QPixmap image,QWidget *parent) : QWidget(parent)
{
    this->image=image;
    initUI();
    initData();
    std::cout<<"DrawingWindow()"<<std::endl;
}

DrawingWindow::~DrawingWindow()
{
    std::cout<<"~DrawingWindow()"<<std::endl;
}

void DrawingWindow::paintEvent(QPaintEvent *e)
{
     QWidget::paintEvent(e);
     QPainter painter(this);
     QPolygon mainPolygon(polygonPoints);
     painter.setPen(myPen);
     painter.drawPixmap(0,0,image);
     if(aviable_complete_polygon)
     {
         if(polygonPoints.size()<=2)
         {
             polygonPoints.clear();//存储多边形点
             this->isStart=true;
             this->aviable_complete_polygon=false;
         }
         else
         {
            //设置填充
            //painter.setBrush(QBrush(Qt::green));
            //设置圆滑边框
            painter.setRenderHint(QPainter::Antialiasing,true);
            //绘制多边形
            painter.drawPolygon(mainPolygon);
            painter.end();
         }
     }
     else
     {
        if(!isStart)
        {
            if(polygonPoints.size()>1)
            {
                //设置圆滑边框
                painter.setRenderHint(QPainter::Antialiasing,true);
                painter.drawPolyline(mainPolygon);
                painter.drawLine(prePoint,tempPoint);
            }
            else
            {
                //设置圆滑边框
                painter.setRenderHint(QPainter::Antialiasing,true);
                painter.drawLine(prePoint,tempPoint);
            }
        }
     }
}

void DrawingWindow::mouseMoveEvent(QMouseEvent * e)
{
    if(isStart)
    {
        return;
    }
    if(aviable_complete_polygon)
    {
        return;
    }
    tempPoint=e->pos();
    this->repaint();
}

void DrawingWindow::mousePressEvent(QMouseEvent * e)
{
    if(aviable_complete_polygon)
    {
        return;
    }
    if(e->button()==Qt::LeftButton)
    {
        if(isStart)
        {
            prePoint=e->pos();
            startPoint=prePoint;
            polygonPoints.push_back(prePoint);
            isStart=false;
        }
        else
        {
            QPoint tempPoint=e->pos();
            if(distancePoint(tempPoint,startPoint)<=5.0)
            {
                aviable_complete_polygon=true;
                this->repaint();
            }
            else
            {
                prePoint=tempPoint;
                polygonPoints.push_back(tempPoint);
            }
        }
    }
    else if(e->button()==Qt::RightButton)
    {
        aviable_complete_polygon=true;
        this->repaint();
    }
}

//重绘
void DrawingWindow::restImage(QPixmap pixmap)
{
    polygonPoints.clear();//存储多边形点
    this->isStart=true;
    this->aviable_complete_polygon=false;
    this->image=pixmap;
    this->repaint();
}

//求两点之间的距离
double DrawingWindow:: distancePoint(QPoint p1, QPoint p2)
{
    double distance=0.0;
    distance=sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y()));
    return distance;
}

QVector<QPoint> DrawingWindow::getPolygon()
{
    return polygonPoints;
}

void DrawingWindow::initUI()
{
    setCursor(Qt::CrossCursor);
    this->setMaximumSize(image.width(),image.height());
    this->setMinimumSize(image.width(),image.height());
}

void DrawingWindow::initData()
{
    aviable_complete_polygon=false;//多边形是否完成
    isStart=true;//第一个坐标点是否存在

    //绘图画笔
    myPen.setWidth(2);
    myPen.setStyle(Qt::SolidLine);
    myPen.setColor(Qt::red);
    polygonPoints.clear();//存储多边形点
}


