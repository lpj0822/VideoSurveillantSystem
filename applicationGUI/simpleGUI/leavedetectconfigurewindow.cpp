#pragma execution_character_set("utf-8")
#include "leavedetectconfigurewindow.h"
#include <QVector>
#include <QPoint>
#include <QMessageBox>
#include <QEvent>
#include <QDebug>

LeaveDetectConfigureWindow::LeaveDetectConfigureWindow(QImage image, QWidget *parent) : QDialog(parent)
{
    this->pixmap = QPixmap::fromImage(image);
    initUI();
    initData();
    initConnect();
}

LeaveDetectConfigureWindow::~LeaveDetectConfigureWindow()
{
    drawArea->deleteLater();
}

void LeaveDetectConfigureWindow::changeEvent(QEvent *event)
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
QList<QPolygonF> LeaveDetectConfigureWindow::getDetectArea()
{
    return area;
}

//最大离岗时间，超过该时间则判定为离岗
int LeaveDetectConfigureWindow::getLeaveMaxTime()
{
    return (maxLeaveTimeBox->value() * 60);
}

void LeaveDetectConfigureWindow::slotOk()
{
    QString message = "";
    QVector<QPoint> tempPolygon = drawArea->getPolygon();
    QPolygonF tempData(tempPolygon);
    if (tempPolygon.size() <= 2)
    {
        message += "划定检测区域不正确!";
    }
    area.push_back(tempData);
    if (message.trimmed() == "")
    {
        accept();
    }
    else
    {
        QMessageBox::information(this, tr("配置参数不合法"), message);
    }

}

void LeaveDetectConfigureWindow::slotRest()
{
    drawArea->restImage(pixmap);
    area.clear();
}

//初始化UI
void LeaveDetectConfigureWindow::initUI()
{
    mainLayout = new QVBoxLayout();

    QHBoxLayout *configureLayout = new QHBoxLayout();
    maxLeaveTimeLabel = new QLabel(tr("最大离岗时间："));
    maxLeaveTimeBox = new QSpinBox();
    maxLeaveTimeBox->setSuffix(tr("分钟"));
    maxLeaveTimeBox->setValue(5);
    maxLeaveTimeBox->setMinimum(5);
    maxLeaveTimeBox->setMaximum(1440);
    maxLeaveTimeBox->setSingleStep(10);
    configureLayout->setSpacing(10);  //设置各个控件之间的边距
    configureLayout->setAlignment(Qt::AlignCenter);
    configureLayout->addWidget(maxLeaveTimeLabel);
    configureLayout->addWidget(maxLeaveTimeBox);


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
    this->setWindowTitle(tr("离岗检测系统参数配置"));
}

//初始化数据
void LeaveDetectConfigureWindow::initData()
{
    area.clear();
}

//事件连接
void LeaveDetectConfigureWindow::initConnect()
{
    connect(okButton, &QPushButton::clicked, this, &LeaveDetectConfigureWindow::slotOk);
    connect(cancelButton, &QPushButton::clicked, this, &LeaveDetectConfigureWindow::reject);
    connect(restButton, &QPushButton::clicked, this, &LeaveDetectConfigureWindow::slotRest);
}

