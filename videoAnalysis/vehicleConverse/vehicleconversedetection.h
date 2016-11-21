/*
 *Vehicle converse detection
 * 车辆逆行检测
 */
#ifndef VEHICLECONVERSEDETECTION_H
#define VEHICLECONVERSEDETECTION_H

#include <QObject>
#include "baseAlgorithm/frameforeground.h"
#include "baseAlgorithm/geometrycalculations.h"
#include "baseAlgorithm/imageprocess.h"
#include "baseAlgorithm/objectrecognition.h"
#include "multipletracking/kalmanmultipletracker.h"
#include "../utility/picturesavethread.h"
#include "conversearea.h"

class VehicleConverseDetection : public QObject
{
    Q_OBJECT

public:
     VehicleConverseDetection(QObject *parent = 0);
    ~VehicleConverseDetection();

    void initDetectData();//初始化检测参数
    int detect(const cv::Mat &frame);//车辆逆行检测
    void startTrcaking(bool isStart);//每个跟踪区域重新开始跟踪

    int getErrorCode();//得到错误码
    std::vector<ConverseArea>&  getDetectArea();//得到检测区域

    QString currentDate;//当前时间

signals:
    void signalMessage(QString message);

private:

    std::vector<cv::Point2f> detectObjectCenter(const cv::Mat& frame, int number);//检测运动目标得到检测区域内的目标中心点
    void tracking(const cv::Mat& roi, const std::vector<cv::Point2f>& centers, int number);//对目标进行多目标跟踪
    void calculateObjectConverse(int number);//匹配区域中是否有逆行目标
    bool isTrcakConverse(const std::vector<cv::Point2f> &points, int myDirection);//判断路径是否逆方向
    int converseArea(int number);//判断某个区域是否逆行
    int converseArea(const cv::Mat& inFrame,int number, QString fileDir="./converse", QString fileName="./converse/image.png");//该区域是否逆行并保存图片

    void drawingDetectArea(cv::Mat &inFrame ,cv::Scalar color=cv::Scalar(255,255,255));//绘制检测区域

    void initData();

private:
    PictureSaveThread *pictureSaveThread;//图片保存类
    ImageProcess *imageProcess;//图像处理算法类
    GeometryCalculations *geometryCalculations;//几何运算类
    FrameForeground *frameForeground;//前景检测类
    ObjectRecognition *objectRecognition;//目标识别
    std::vector<KalmanMultipleTracker *> allMultipleTrackers;//多目标跟踪类

    std::vector<ConverseArea> detectArea;//整个检测区域
    std::vector< std::vector<cv::Point> > pointsArea;
    std::vector<int> areaDirection;//每个区域的正常行驶方向

    int minConversePointNum;//运动路径中至少多少个逆行点
    float crossMatchMaxValue;//两矩形相交的临界值，大于这个值就认为匹配
    float minBox;//检测的最小目标面积大小
    int minSize;//最小检测目标
    QString savePictureDir;//保存截图的路径
    QString savePictureFileName;
    QString saveClassiferPath;//分类器保存路径

    bool isDrawObject;

    int saveDeltaTime;//两帧之间保存一张图片的间隔s

    int errorCode;//错误码
    bool isFirstRun;//第一次运行

private:
    void init();//初始化
    void saveConfig();
    void loadConfig();


};

#endif // VEHICLECONVERSEDETECTION_H
