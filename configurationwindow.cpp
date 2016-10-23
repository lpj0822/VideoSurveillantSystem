#pragma execution_character_set("utf-8")
#include "configurationwindow.h"
#include <QVector>
#include <QPoint>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QEvent>
#include <QFileInfo>
#include <QDebug>

ConfigurationWindow::ConfigurationWindow(QImage image,QWidget *parent) : QDialog(parent)
{
   this->pixmap=QPixmap::fromImage(image);
   initUI();
   initData();
   std::cout<<"ConfigurationWindow()"<<std::endl;
}

ConfigurationWindow::~ConfigurationWindow()
{
    if(drawArea)
    {
        delete drawArea;
        drawArea=NULL;
    }
    std::cout<<"~ConfigurationWindow()"<<std::endl;
}

void ConfigurationWindow::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    switch(event->type())
    {
    case QEvent::LanguageChange:
        break;
    case QEvent::Close:
        reject();
        break;
    default:
       break;
    }
}

//得到逆行检测区域
QList<QPolygonF> ConfigurationWindow::getDetectArea()
{
    return area;
}

//车辆正常行驶方向
QList<int> ConfigurationWindow::getDirection()
{
    QList<int> areaDirection;
    for(int loop=0;loop<area.size();loop++)
    {
        areaDirection.push_back(directionBox->currentData().toInt());
    }
    return areaDirection;
}

//运动路径中至少多少个逆行点
int ConfigurationWindow::getMinConverseNum()
{
    return minConverseNumBox->value();
}

//两帧之间目标最大的移动距离
double ConfigurationWindow::getDistThreshold()
{
    return distThresholdBox->value();
}

//允许目标消失的最大帧数
int ConfigurationWindow::getMaxAllowedSkippedFrames()
{
    return maxAllowedSkippedFramesBox->value();
}

//跟踪轨迹的最大长度
int ConfigurationWindow::getMaxTraceLength()
{
    return maxTraceLengthBox->value();
}

//前景检测算法类型
int ConfigurationWindow::getAlgorithm()
{
    return algorithmBox->currentData().toInt();
}
//是否进行图像滤波
bool ConfigurationWindow::getIsImageFilter()
{
    return isImageFilterBox->isChecked();
}

//采用的图像滤波器
int ConfigurationWindow::getImageFilterType()
{
    return filterTypeBox->currentData().toInt();
}

//最小检测区域
int ConfigurationWindow::getMinObjectArea()
{
    return minObjectAreaBox->value();
}

//区域相交度最小值
float ConfigurationWindow::getAreaCrossValue()
{
    return (float)areaCrossValueBox->value();
}

//截图保存路径
QString ConfigurationWindow::getSavePathDir()
{
    return myDir;
}

QString ConfigurationWindow::getSavePathFileName()
{
    return myFileName;
}


void ConfigurationWindow::slotOpenSaveDialog()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,tr("保存图片路径"), "./image.png", "image files(*.png *.jpg)");
    savePictureEdit->setText(fileName);
}

void ConfigurationWindow::slotOk()
{
    QString message="";
    QVector<QPoint> tempPolygon=drawArea->getPolygon();
    QPolygonF tempData(tempPolygon);
    if(tempPolygon.size()<=2)
    {
        message+="划定检测区域不正确!";
    }
    area.push_back(tempData);
    if(message.trimmed()=="")
    {
        accept();
    }
    else
    {
        QMessageBox::information(this, tr("配置参数不合法"), message);
    }

}

void ConfigurationWindow::slotCancel()
{
    reject();
}

void ConfigurationWindow::slotRest()
{
    drawArea->restImage(pixmap);
}

//初始化UI
void ConfigurationWindow::initUI()
{
    mainLayout=new QVBoxLayout();
    QGridLayout *centerLayout=new QGridLayout();

    QGridLayout *leftLayout=new QGridLayout();

    directionLabel=new QLabel(tr("正常行驶方向："));//正常行车方向
    directionBox=new QComboBox();
    directionBox->addItem(tr("向上行驶"),1);
    directionBox->addItem(tr("向下行驶"),2);
    directionBox->addItem(tr("向左行驶"),3);
    directionBox->addItem(tr("向右行驶"),4);
    directionBox->addItem(tr("无行驶方向"),0);

    minConverseNumLabel=new QLabel("路径中最小逆行点数：");//运动路径中至少多少个逆行点
    minConverseNumBox=new QSpinBox();
    minConverseNumBox->setMinimum(2);
    minConverseNumBox->setMaximum(10);
    minConverseNumBox->setSingleStep(1);
    minConverseNumBox->setValue(10);

    distThresholdLabel=new QLabel(tr("两帧之间目标最大的移动距离："));//两帧之间目标最大的移动距离
    distThresholdBox=new QDoubleSpinBox();
    distThresholdBox->setMinimum(30.0);
    distThresholdBox->setMaximum(100.0);
    distThresholdBox->setSingleStep(10.0);
    distThresholdBox->setValue(50.0);

    maxAllowedSkippedFramesLabel=new QLabel(tr("允许目标消失的最大帧数："));//允许目标消失的最大帧数
    maxAllowedSkippedFramesBox=new QSpinBox();
    maxAllowedSkippedFramesBox->setMinimum(5);
    maxAllowedSkippedFramesBox->setMaximum(50);
    maxAllowedSkippedFramesBox->setSingleStep(2);
    maxAllowedSkippedFramesBox->setValue(8);

    maxTraceLengthLabel=new QLabel(tr("跟踪轨迹的最大长度："));//跟踪轨迹的最大长度
    maxTraceLengthBox=new QSpinBox();
    maxTraceLengthBox->setMinimum(10);
    maxTraceLengthBox->setMaximum(30);
    maxTraceLengthBox->setSingleStep(5);
    maxTraceLengthBox->setValue(10);

    algorithmLabel=new QLabel(tr("前景检测算法："));
    algorithmBox=new QComboBox();//设置最大离岗检测此时
    algorithmBox->addItem(tr("混合高斯模型"),0);
    algorithmBox->addItem(tr("PBAS模型"),1);
    algorithmBox->addItem(tr("VIBE模型"),2);

    imageFilterLabel=new QLabel("图像滤波器设置：");//图像滤波设置
    isImageFilterBox=new QCheckBox(tr("是否滤波"));
    filterTypeBox=new QComboBox();
    filterTypeBox->addItem(tr("方框滤波器"),1);
    filterTypeBox->addItem(tr("均值滤波器"),2);
    filterTypeBox->addItem(tr("高斯滤波器"),3);
    filterTypeBox->addItem(tr("中值滤波器"),4);
    filterTypeBox->addItem(tr("双边滤波器"),5);

    minObjectAreaLabel=new QLabel(tr("检测的最小目标面积："));
    minObjectAreaBox=new QSpinBox();//能检测的最小目标面积
    minObjectAreaBox->setMinimum(100);
    minObjectAreaBox->setMaximum(pixmap.width()*pixmap.height());
    minObjectAreaBox->setSingleStep(100);
    minObjectAreaBox->setValue(300);

    areaCrossValueLabel=new QLabel(tr("区域相交度最小值："));
    areaCrossValueBox=new QDoubleSpinBox();//区域相交度最小值
    areaCrossValueBox->setMinimum(0.01);
    areaCrossValueBox->setMaximum(1.0);
    areaCrossValueBox->setSingleStep(0.05);
    areaCrossValueBox->setValue(0.1);

    savePictueLabel=new QLabel(tr("逆行截图保存路径："));
    savePictureEdit=new QLineEdit();//设置保存截图的路径
    savePictureEdit->setText(tr("./converse/image.png"));
    savePictureEdit->setReadOnly(true);
    savePathButton=new QPushButton(tr("..."));//打开保存对话框的按钮
    connect(savePathButton,&QPushButton::clicked,this,&ConfigurationWindow::slotOpenSaveDialog);

    leftLayout->addWidget(directionLabel,0,0);
    leftLayout->addWidget(directionBox,0,1);
    leftLayout->addWidget(minConverseNumLabel,1,0);
    leftLayout->addWidget(minConverseNumBox,1,1);
    leftLayout->addWidget(distThresholdLabel,2,0);
    leftLayout->addWidget(distThresholdBox,2,1);
    leftLayout->addWidget(maxAllowedSkippedFramesLabel,3,0);
    leftLayout->addWidget(maxAllowedSkippedFramesBox,3,1);
    leftLayout->addWidget(maxTraceLengthLabel,4,0);
    leftLayout->addWidget(maxTraceLengthBox,4,1);
    leftLayout->addWidget(algorithmLabel,5,0);
    leftLayout->addWidget(algorithmBox,5,1);
    leftLayout->addWidget(imageFilterLabel,6,0);
    leftLayout->addWidget(isImageFilterBox,6,1);
    leftLayout->addWidget(filterTypeBox,6,2);
    leftLayout->addWidget(minObjectAreaLabel,7,0);
    leftLayout->addWidget(minObjectAreaBox,7,1);
    leftLayout->addWidget(areaCrossValueLabel,8,0);
    leftLayout->addWidget(areaCrossValueBox,8,1);
    leftLayout->addWidget(savePictueLabel,9,0);
    leftLayout->addWidget(savePictureEdit,9,1);
    leftLayout->addWidget(savePathButton,9,2);
    leftLayout->setColumnStretch(0,10);
    leftLayout->setColumnStretch(1,10);
    leftLayout->setColumnStretch(2,1);

    drawArea=new DrawingWindow(pixmap);
    drawArea->setMouseTracking(true);
    this->widthWindow=drawArea->width()+340;
    this->heightWindow=drawArea->height()+90;

    centerLayout->addLayout(leftLayout,0,0);
    centerLayout->addWidget(drawArea,0,1);
    centerLayout->setColumnStretch(0,320);
    centerLayout->setColumnStretch(1,drawArea->width());

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->setSpacing(widthWindow/6);

    okButton=new QPushButton(tr("确定"));
    connect(okButton,&QPushButton::clicked,this,&ConfigurationWindow::slotOk);
    cancelButton=new QPushButton(tr("取消"));
    connect(cancelButton,&QPushButton::clicked,this,&ConfigurationWindow::slotCancel);
    restButton=new QPushButton(tr("重绘"));//重绘
    connect(restButton,&QPushButton::clicked,this,&ConfigurationWindow::slotRest);

    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(restButton);

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);

    this->setMaximumSize(widthWindow,heightWindow);
    this->setMinimumSize(widthWindow,heightWindow);
    this->setWindowTitle(tr("车辆逆行检测系统参数配置"));
}

//初始化数据
void ConfigurationWindow::initData()
{
    area.clear();

    myDir="./converse";//目录名和文件名
    myFileName="image";
}

