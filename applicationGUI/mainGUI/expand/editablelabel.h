#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QToolButton>

class EditableLabel : public QLabel
{
public:
    EditableLabel(QWidget *parent = 0);
    void ClearPoints();
    void SetNewQImage(QImage &image);
    QList<QPolygonF> GetPoints();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

private:
    void DrawPixmap();
    bool MousePressed;
    bool NearPiont;
    bool MovePoint;
    bool TrackMouse;
    bool NearFirstPiont;
    int PointIndex;
    int PointsIndex;
    QPolygonF mypoints;
    QList<QPolygonF> PointsList;
    QPointF FirstPoint;

    QPixmap mp;
    QPixmap temppixmap;
};

#endif // EDITABLELABEL_H
