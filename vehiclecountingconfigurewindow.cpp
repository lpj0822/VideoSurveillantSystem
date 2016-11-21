#pragma execution_character_set("utf-8")
#include "vehiclecountingconfigurewindow.h"
#include <QVector>
#include <QPoint>
#include <QMessageBox>
#include <QEvent>
#include <QDebug>

VehicleCountingConfigureWindow::VehicleCountingConfigureWindow(QImage image, QWidget *parent) : QDialog(parent)
{
    this->pixmap = QPixmap::fromImage(image);
    initUI();
    initData();
    initConnect();
}

VehicleCountingConfigureWindow::~VehicleCountingConfigureWindow()
{
    drawArea->deleteLater();
}

void VehicleCountingConfigureWindow::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    switch (event->type())
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

//得到检测区域
QList<QPolygonF> VehicleCountingConfigureWindow::getDetectArea()
{
    return area;
}

//车辆正常行驶方向
QList<int> VehicleCountingConfigureWindow::getDirection()
{
    QList<int> areaDirection;
    for(int loop=0;loop<area.size();loop++)
    {
        areaDirection.push_back(directionBox->currentData().toInt());
    }
    return areaDirection;
}

void VehicleCountingConfigureWindow::slotOk()
{
    QString message = "";
    QVector<QPoint> tempPolygon = drawArea->getPolygon();
    QPolygonF tempData(tempPolygon);
    if (tempPolygon.size() <= 2)
    {
        message += "划定检测区域不正确!";
    }
    area.push_back(tempData);
    if(message.trimmed() == "")
    {
        accept();
    }
    else
    {
        QMessageBox::information(this, tr("配置参数不合法"), message);
    }

}

void VehicleCountingConfigureWindow::slotRest()
{
    drawArea->restImage(pixmap);
}

//初始化UI
void VehicleCountingConfigureWindow::initUI()
{
    mainLayout = new QVBoxLayout();

    QHBoxLayout *configureLayout = new QHBoxLayout();
    directionLabel = new QLabel(tr("正常行驶方向："));//正常行车方向
    directionBox = new QComboBox();
    directionBox->addItem(tr("向上行驶"), 1);
    directionBox->addItem(tr("向下行驶"), 2);
    directionBox->addItem(tr("向左行驶"), 3);
    directionBox->addItem(tr("向右行驶"), 4);
    directionBox->addItem(tr("无行驶方向"), 0);
    configureLayout->setSpacing(10);  //设置各个控件之间的边距
    configureLayout->setAlignment(Qt::AlignCenter);
    configureLayout->addWidget(directionLabel);
    configureLayout->addWidget(directionBox);


    drawArea = new DrawingWindow(pixmap);
    drawArea->setMouseTracking(true);
    this->widthWindow=drawArea->width() + 20;
    this->heightWindow=drawArea->height() + 90;

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->setSpacing(widthWindow / 6);
    okButton = new QPushButton(tr("确定"));
    cancelButton = new QPushButton(tr("取消"));
    restButton = new QPushButton(tr("重绘"));//重绘
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(restButton);

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(drawArea);
    mainLayout->addLayout(configureLayout);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);

    this->setMaximumSize(widthWindow, heightWindow);
    this->setMinimumSize(widthWindow, heightWindow);
    this->setWindowTitle(tr("车流量统计系统参数配置"));
}

//初始化数据
void VehicleCountingConfigureWindow::initData()
{
    area.clear();
}

//事件连接
void VehicleCountingConfigureWindow::initConnect()
{
    connect(okButton, &QPushButton::clicked, this, &VehicleCountingConfigureWindow::slotOk);
    connect(cancelButton, &QPushButton::clicked, this, &VehicleCountingConfigureWindow::reject);
    connect(restButton, &QPushButton::clicked, this, &VehicleCountingConfigureWindow::slotRest);
}
