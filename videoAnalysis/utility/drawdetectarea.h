#ifndef DRAWDETECTAREA_H
#define DRAWDETECTAREA_H

#include <QObject>
#include "polygonarea.h"
#include "baseAlgorithm/imageprocess.h"

class DrawDetectArea : public QObject
{
    Q_OBJECT

public:
    DrawDetectArea(QObject *parent = 0);
    ~DrawDetectArea();

    void drawingDetectArea(cv::Mat &inFrame, const std::vector<PolygonArea> &detectArea, cv::Scalar color=cv::Scalar(255,255,255));

signals:

public slots:

private:
    ImageProcess imageProcess;
    //整个检测区域
};

#endif // DRAWDETECTAREA_H
