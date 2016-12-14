#pragma execution_character_set("utf-8")
#include "simplecontrolwindow.h"
#include <QHBoxLayout>
#include <QDebug>

SimpleControlWindow::SimpleControlWindow(QWidget *parent)
    : QWidget(parent)
{
    init();
    initUI();
    initConnect();
}

SimpleControlWindow::~SimpleControlWindow()
{
    leaveDetectWindow->deleteLater();
    intrudeDetectWindow->deleteLater();
    vehicleConverseWindow->deleteLater();
    vehicleCountingWindow->deleteLater();
}

void SimpleControlWindow::slotOK()
{

}

void SimpleControlWindow::init()
{

}

void SimpleControlWindow::initUI()
{
    tabweight=new QTabWidget();
    leaveDetectWindow = new LeaveDetectControlWindow();
    intrudeDetectWindow = new IntrudeDetectControlWindow();
    vehicleConverseWindow = new VehicleConverseControlWindow();
    vehicleCountingWindow = new VehicleCountingControlWindow();

    tabweight->addTab(leaveDetectWindow, tr("离岗检测系统"));
    tabweight->addTab(intrudeDetectWindow, tr("入侵检测系统"));
    tabweight->addTab(vehicleConverseWindow, tr("车辆逆行检测系统"));
    tabweight->addTab(vehicleCountingWindow, tr("车流量统计系统"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); // 创建QDialogButtonBox
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));//将buttonbox中的ok 变成汉化
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    mainLayout=new QVBoxLayout();
    mainLayout->addWidget(tabweight);
    mainLayout->addWidget(buttonBox);

    this->setLayout(mainLayout);
    this->setMinimumSize(1020,820);
    this->setWindowTitle(tr("智能视频监控系统"));
}

void SimpleControlWindow::initConnect()
{
    connect(buttonBox,&QDialogButtonBox::accepted, this, &SimpleControlWindow::slotOK);  // 为button定义连接信号槽
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SimpleControlWindow::close);
}
