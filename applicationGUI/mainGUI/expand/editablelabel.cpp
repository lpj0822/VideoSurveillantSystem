#include "editablelabel.h"
#include <QDebug>

EditableLabel::EditableLabel(QWidget *parent):
    QLabel(parent),
    MousePressed(false),
    NearPiont(false),
    MovePoint(false),
    TrackMouse(false),
    FirstPoint(0,0),
    NearFirstPiont(false),
    PointsIndex(0)
{
    this->setMouseTracking(true);
}

void EditableLabel::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton){
        MousePressed = true;
        if(MovePoint){
            TrackMouse = true;
            NearPiont = true;
            setCursor(Qt::ClosedHandCursor);
            return;
        }

        if(NearFirstPiont){
            FirstPoint = QPointF(0,0);
            QPolygonF points;
            points.append(FirstPoint);
            PointsList.append(points);
            setCursor(Qt::OpenHandCursor);
            DrawPixmap();
            NearFirstPiont = false;
            MovePoint = true;
            for(int i=0;i<PointsList.count()-1;i++){
                for(int j=0;j<PointsList[i].count();j++){
                    QPointF point = e->localPos() - PointsList[i][j];
                    if(point.manhattanLength() <= 13){
                        PointsIndex = i;
                        PointIndex = j;
                        return;
                    }
                }
            }
        }

        if(FirstPoint.isNull()){
            FirstPoint = e->localPos();
            if(PointsList.isEmpty()){
                QPolygonF points;
                points.append(FirstPoint);
                PointsList.append(points);
            }
            else {
                PointsList.last()[0] = FirstPoint;
                NearPiont = false;
            }
            DrawPixmap();
            return;
        }

        PointsList.last().append(e->localPos());
        DrawPixmap();
    }
}

void EditableLabel::mouseMoveEvent(QMouseEvent *e)
{
    if(PointsList.isEmpty())
        return;
    if(!this->rect().contains(e->pos())){
        return;
    }

    if(TrackMouse){
        PointsList[PointsIndex][PointIndex] = e->pos();
        DrawPixmap();
    }
    else {
        for(int i=0;i<PointsList.count()-1;i++){
            for(int j=0;j<PointsList[i].count();j++){
                QPointF point = e->localPos() - PointsList[i][j];
                if(point.manhattanLength() <= 13){
                    setCursor(Qt::OpenHandCursor);
                    MovePoint = true;
                    PointsIndex = i;
                    PointIndex = j;
                    return;
                }
            }
        }
        MovePoint = false;
        if(PointsList.last().size()>2){
            QPointF point = e->localPos() - FirstPoint;
            if(point.manhattanLength() <= 9){
                setCursor(Qt::CrossCursor);
                NearFirstPiont = true;
            }
            else {
                if(!MousePressed){
                    setCursor(Qt::ArrowCursor);
                    NearPiont = false;
                }
            }
        }
        else {
            setCursor(Qt::ArrowCursor);
            return;
        }
    }
}

void EditableLabel::DrawPixmap()
{
    QPainter painter;
    temppixmap = mp;
    painter.begin(&temppixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QColor("#3CFF55"));
    for(int i=0; i<PointsList.size()-1; i++){
        QPolygonF drawpoints = PointsList[i];
        drawpoints.append(drawpoints.at(0));
        QPainterPath path;
        path.addPolygon(PointsList[i]);
        painter.fillPath(path,QBrush(QColor(163,250,148,80)));

        foreach (QPointF var, PointsList[i]) {
                painter.drawEllipse(var, 3, 3);
        }

        QPen pen(QColor("#3CFF55"), 2 ,Qt::DashLine);
        painter.setPen(pen);
        painter.drawPolyline(QPolygonF(drawpoints));
    }
    if(!PointsList.isEmpty()){
        if(!PointsList.last().isEmpty()){
            QPolygonF drawpoints = PointsList.last();

            painter.setBrush(QColor("#3CFF55"));
            foreach (QPointF var, PointsList.last()) {
                if(!var.isNull())
                    painter.drawEllipse(var, 3, 3);
            }

            QPen pen(QColor("#3CFF55"), 2 ,Qt::DashLine);
            painter.setPen(pen);
            painter.drawPolyline(QPolygonF(drawpoints));
        }
    }
    painter.end();
    this->update();
}

void EditableLabel::mouseReleaseEvent(QMouseEvent *)
{
    MousePressed = false;
    TrackMouse = false;
    if(NearPiont){
        setCursor(Qt::OpenHandCursor);
    }
}

void EditableLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(QPoint(0,0),temppixmap);
}

void EditableLabel::ClearPoints()
{
    mypoints.clear();
    PointsList.clear();
    DrawPixmap();

    MousePressed = false;
    NearPiont = false;
    MovePoint = false;
    TrackMouse = false;
    FirstPoint = QPointF(0,0);
    NearFirstPiont = false;
    PointsIndex = 0;
}

void EditableLabel::SetNewQImage(QImage &image)
{
    mp = QPixmap::fromImage(image);
    DrawPixmap();
}

QList<QPolygonF> EditableLabel::GetPoints()
{
    if(!PointsList.isEmpty()){
        PointsList.removeLast();
    }

    for(auto var:PointsList){
        qDebug()<<var;
    }

    return PointsList;
}
