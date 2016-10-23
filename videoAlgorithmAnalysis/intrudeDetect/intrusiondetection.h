#ifndef INTRUSIONDETECTION_H
#define INTRUSIONDETECTION_H

#include <QObject>
#include "../baseAlgorithm/frameforeground.h"
#include "../baseAlgorithm/geometrycalculations.h"
#include "../baseAlgorithm/imageprocess.h"
#include "../picturesavethread.h"
#include "../errorcodestring.h"
#include "intrusionarea.h"

class IntrusionDetection : public QObject
{
    Q_OBJECT
public:
    IntrusionDetection(QObject *parent = 0);
    ~IntrusionDetection();

    void initDetectData();//初始化检测参数

    int detect(cv::Mat &frame);//入侵检测

    int getErrorCode();//得到错误码
    std::vector<IntrusionArea>  getDetectArea();//得到入侵区域

    QString currentDate;//当前时间

signals:

public slots:

private:
    std::vector<cv::Rect> detectObject(cv::Mat &frame);//检测运动目标

    void matchDetectAllArea(std::vector<cv::Rect> vectorRect);//通过检测出的目标匹配目标区域
    void matchDetectArea(std::vector<cv::Rect> vectorRect,int number);//对某一个区域匹配目标

    int  intrusionArea(int number);//判断某个区域是否入侵

    QList<int> allIntrusionArea(cv::Mat inFrame,QString fileDir="./intrude",QString fileName="./intrude/image.png");//得到入侵区域并保存图片
    int intrusionArea(cv::Mat inFrame,int number,QString fileDir="./intrude",QString fileName="./intrude/image.png");//该区域是否入侵并保存图片

    void drawingDetectArea(cv::Mat &inFrame ,cv::Scalar color=cv::Scalar(255,255,255));//绘制入侵区域

private:
    GeometryCalculations *geometryCalculations;//几何运算类
    FrameForeground *frameForeground;//前景检测类
    PictureSaveThread *pictureSaveThread;//图片保存类
    ImageProcess *imageProcess;//图像处理类

    std::vector<IntrusionArea> detectArea;//整个检测区域
    std::vector< std::vector<cv::Point> > pointsArea;

    float crossMatchMaxValue;//两矩形相交的临界值，大于这个值就认为匹配
    float minBox;//检测的最小目标面积大小
    QString savePictureDir;//保存截图的路径

    QString savePictureFileName;

    int deltaTime;//两次入侵中间隔多长时间保存一张图片s

    bool isDrawObject;

    ErrorCodeString errorCodeString;

    int errorCode;//错误码
    bool isFirstRun;//第一次运行

    void initData();//初始化数据

private:
    void saveConfig();
    void loadConfig();
};

#endif // INTRUSIONDETECTION_H
