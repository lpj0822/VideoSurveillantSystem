#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QDir>
#include <QPolygonF>

#include "drawingwindow.h"

class ConfigurationWindow : public QDialog
{
    Q_OBJECT
public:
    ConfigurationWindow(QImage image,QWidget *parent = 0);
    ~ConfigurationWindow();

    QList<QPolygonF> getDetectArea();//得到逆行检测区域

    QList<int> getDirection();//车辆正常行驶方向
    int getMinConverseNum();//运动路径中至少多少个逆行点

    double getDistThreshold();//两帧之间目标最大的移动距离
    int getMaxAllowedSkippedFrames();//允许目标消失的最大帧数
    int getMaxTraceLength();//跟踪轨迹的最大长度

    int getAlgorithm();//前景检测算法类型
    bool getIsImageFilter();//是否进行图像滤波
    int getImageFilterType();//采用的图像滤波器
    int getMinObjectArea();//最小检测区域
    float getAreaCrossValue();//区域相交度最小值
    QString getSavePathDir();//截图保存路径
    QString getSavePathFileName();


signals:

public slots:
    void slotOpenSaveDialog();//打开保存文件对话框
    void slotOk();
    void slotCancel();
    void slotRest();

protected:
    void changeEvent(QEvent *event);

private:

    QVBoxLayout *mainLayout;//主布局

    QPixmap pixmap;//绘制区域画面
    DrawingWindow *drawArea;//绘图区域

    QLabel *directionLabel;//正常行车方向
    QComboBox *directionBox;

    QLabel *minConverseNumLabel;//运动路径中至少多少个逆行点
    QSpinBox *minConverseNumBox;

    QLabel *distThresholdLabel;//两帧之间目标最大的移动距离
    QDoubleSpinBox *distThresholdBox;

    QLabel *maxAllowedSkippedFramesLabel;//允许目标消失的最大帧数
    QSpinBox *maxAllowedSkippedFramesBox;

    QLabel *maxTraceLengthLabel;//跟踪轨迹的最大长度
    QSpinBox *maxTraceLengthBox;

    QLabel *minObjectAreaLabel;
    QSpinBox *minObjectAreaBox;//能检测的最小目标面积

    QLabel *areaCrossValueLabel;
    QDoubleSpinBox *areaCrossValueBox;//区域相交度最小值

    QLabel *algorithmLabel;
    QComboBox *algorithmBox;//设置前景检测算法类型

    QLabel *imageFilterLabel;//图像滤波设置
    QCheckBox *isImageFilterBox;
    QComboBox *filterTypeBox;

    QLabel *savePictueLabel;
    QLineEdit *savePictureEdit;//设置保存截图的路径
    QPushButton *savePathButton;//打开保存对话框的按钮

    QPushButton *okButton;//确定按钮
    QPushButton *cancelButton;//取消按钮
    QPushButton *restButton;//重绘

    QList<QPolygonF> area;//入侵检测区域

    QString myDir;//目录名和文件名
    QString myFileName;

    int widthWindow;//窗口长和宽
    int heightWindow;

    QDir makeDir;

    void initUI();//初始化话UI
    void initData();//初始化数据

};

#endif // CONFIGURATIONWINDOW_H
