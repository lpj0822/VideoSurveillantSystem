#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include "applicationGUI/mainGUI/expand/wexpand.h"

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), leftTabXxpanded(true)
{
    InitGui();
    initData();
    initConnect();
}

MainWindow::~MainWindow()
{
    if(ProcessDeviate)//ensure the process is closed
    {
        ProcessDeviate->close();
        delete ProcessDeviate;
        ProcessDeviate = NULL;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    WriteSetting();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if (leftTabXxpanded){
        LeftTab->setMaximumWidth(170);
        leftTabminmumwidth = LeftTab->width();
    }
    QMainWindow::resizeEvent(e);
}

void MainWindow::addLogMsg(const QString &str)
{
    TextEvent->appendPlainText(str);
}

void MainWindow::SlotLeave()
{
    MainWindow *mainwindow = qApp->property("MainWindow").value<MainWindow *>();
    mainwindow->addLogMsg("运行离岗检测模块程序");
}

void MainWindow::SlotVehicleRetrograde()
{
    MainWindow *mainwindow = qApp->property("MainWindow").value<MainWindow *>();
    mainwindow->addLogMsg("运行车辆逆行模块程序");
    CentralWidget->setCurrentIndex(1);
}

void MainWindow::InitGui()
{
    InitMenuBar();
    InitMain();
    InitLeft();
    InitCentral();
    InitRight();
}

void MainWindow::InitMenuBar()
{
    InitAction();
    AboveMenuBar = new QMenuBar(this);
    FunctionMenu = new QMenu(AboveMenuBar);
    AboveMenuBar->addAction(FunctionMenu->menuAction());

    FunctionMenu->addAction(actAbout);
    FunctionMenu->addAction(actUserManual);
    FunctionMenu->setTitle("设置");

    this->setMenuBar(AboveMenuBar);
}

void MainWindow::InitMain()
{

    MainLayout = new QGridLayout();
    MainLayout->setSpacing(1);
    MainLayout->setContentsMargins(1, 1, 1, 1);

    LeftTab = new QTabWidget();
    cenitem = new QWidget(this);

    MainLayout->addWidget(LeftTab, 0, 0);
    MainLayout->addWidget(cenitem, 0, 1);

    MainLayout->setColumnStretch(0, 0);
    MainLayout->setColumnStretch(1, 1);
    QWidget *item = new QWidget(this);
    item->setLayout(MainLayout);
    this->setCentralWidget(item);
}

void MainWindow::InitLeft()
{
    QFrame *TabFunc = new QFrame();
    InitLeftToolBtn(TabFunc);
    LeftTab->addTab(TabFunc, "系统功能");
    LeftTab->setCurrentWidget(TabFunc);
}

void MainWindow::InitCentral()
{

    QHBoxLayout *cenLayout = new QHBoxLayout(cenitem);
    cenLayout->setMargin(0);
    cenLayout->setSpacing(0);
    WExpand *expand = new WExpand();

    initStackCenterWidget();

    cenLayout->addWidget(expand);
    cenLayout->addWidget(CentralWidget);
    cenitem->setLayout(cenLayout);

    customAnimation = new CustomAnimation(LeftTab, cenitem);
    connect(expand, &WExpand::signalStatusChangeed,[&](bool expanded){
        QPropertyAnimation *animation = new  QPropertyAnimation(LeftTab, "maximumWidth",this);
        if(expanded){
            animation->setStartValue(LeftTab->width());
            animation->setEndValue(leftTabminmumwidth);
        }
        else{
            animation->setStartValue(LeftTab->width());
            animation->setEndValue(0);
        }
        leftTabXxpanded = expanded;
        animation->setDuration(300);
        animation->start();
        connect(animation,&QPropertyAnimation::finished,[animation](){
            animation->deleteLater();
            qDebug()<<"animation->deleteLater();";
        });
    });
}

void MainWindow::InitRight()
{
    RightDock = new QDockWidget("系统信息", this);
    RightDock->setMinimumWidth(150);
    RightDock->setContentsMargins(0, 0, 0, 0);

    RightTab = new QTabWidget();
    TextEvent = new QPlainTextEdit(RightTab);
    RightTab->addTab(TextEvent, "事件");

    RightDock->setWidget(RightTab);
    this->addDockWidget(Qt::RightDockWidgetArea, RightDock);
}

void MainWindow::InitAction()
{
    actAbout = new QAction("关于",this);
    actUserManual = new QAction("系统说明",this);
}

void MainWindow::initStackCenterWidget()
{
    CentralWidget = new QStackedWidget(this);

    mainCentralTab = new QTabWidget();
    mainCentralTab->setTabPosition(QTabWidget::West);

    mainWidget = new MainWidget();
    mainCentralTab->addTab(mainWidget, "视频播放");

    vehicleConverseWidget = new VehicleConverseWidget(this);
    vehicleConverseWidget->hide();

    CentralWidget->addWidget(mainCentralTab);
    CentralWidget->addWidget(vehicleConverseWidget);
    CentralWidget->setCurrentIndex(0);
}

void MainWindow::InitLeftToolBtn(QFrame *area)
{
    area->setContentsMargins(0,0,0,0);

    QGridLayout *gridLayout = new QGridLayout(area);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    ToolBtnLeave = new QToolButton(area);
    ToolBtnAreaIntrusion = new QToolButton(area);
    ToolBtnTarget = new QToolButton(area);
    ToolBtnDystropy = new QToolButton(area);
    ToolBtnPopulation = new QToolButton(area);
    ToolBtnTunnelPedestrian = new QToolButton(area);
    ToolBtnTrafficFlow = new QToolButton(area);
    ToolBtnTurnLane = new QToolButton(area);
    ToolBtnIllegalPark = new QToolButton(area);
    ToolBtnVehicleRetrograde = new QToolButton(area);
    ToolBtnVehicleSpeed = new QToolButton(area);
    ToolBtnTrafficAccident = new QToolButton(area);

    ToolBtnLeave->setIcon(QIcon(":/images/Deviate.png"));
    ToolBtnLeave->setIconSize(QSize(32, 32));
    ToolBtnLeave->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnLeave->setText("离岗检测");
    ToolBtnLeave->setVisible(true);

    ToolBtnAreaIntrusion->setIcon(QIcon(":/images/areaIntrude.png"));
    ToolBtnAreaIntrusion->setIconSize(QSize(32, 32));
    ToolBtnAreaIntrusion->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnAreaIntrusion->setText("区域入侵");
    ToolBtnAreaIntrusion->setVisible(true);

    ToolBtnVehicleRetrograde->setIcon(QIcon(":/images/VehicleRetrograde.png"));
    ToolBtnVehicleRetrograde->setIconSize(QSize(32, 32));
    ToolBtnVehicleRetrograde->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnVehicleRetrograde->setText("车辆逆行");
    ToolBtnVehicleRetrograde->setVisible(true);

    ToolBtnTrafficFlow->setIcon(QIcon(":images/TrafficFlow.png"));
    ToolBtnTrafficFlow->setIconSize(QSize(32, 32));
    ToolBtnTrafficFlow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnTrafficFlow->setText("车流量统计");
    ToolBtnTrafficFlow->setVisible(true);

    ToolBtnTarget->setIcon(QIcon(":/images/Target.png"));
    ToolBtnTarget->setIconSize(QSize(32, 32));
    ToolBtnTarget->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnTarget->setText("目标检测");
    ToolBtnTarget->setVisible(true);

    ToolBtnPopulation->setIcon(QIcon(":/images/Population.png"));
    ToolBtnPopulation->setIconSize(QSize(32, 32));
    ToolBtnPopulation->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnPopulation->setText("室内人数");
    ToolBtnPopulation->setVisible(true);

    ToolBtnTunnelPedestrian->setIcon(QIcon(":/images/TunnelPedestrian.png"));
    ToolBtnTunnelPedestrian->setIconSize(QSize(32, 32));
    ToolBtnTunnelPedestrian->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnTunnelPedestrian->setText("隧道行人");
    ToolBtnTunnelPedestrian->setVisible(true);

    ToolBtnTurnLane->setIcon(QIcon(":/images/TurnLane.png"));
    ToolBtnTurnLane->setIconSize(QSize(32, 32));
    ToolBtnTurnLane->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnTurnLane->setText("车辆变道");
    ToolBtnTurnLane->setVisible(true);

    ToolBtnIllegalPark->setIcon(QIcon(":/images/IllegalPark.png"));
    ToolBtnIllegalPark->setIconSize(QSize(32, 32));
    ToolBtnIllegalPark->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnIllegalPark->setText("违章停车");
    ToolBtnIllegalPark->setVisible(true);

    ToolBtnVehicleSpeed->setIcon(QIcon(":/images/VehicleSpeed.png"));
    ToolBtnVehicleSpeed->setIconSize(QSize(32, 32));
    ToolBtnVehicleSpeed->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnVehicleSpeed->setText("车辆速度");
    ToolBtnVehicleSpeed->setVisible(true);

    ToolBtnDystropy->setIcon(QIcon(":/images/Dystropy.png"));
    ToolBtnDystropy->setIconSize(QSize(32, 32));
    ToolBtnDystropy->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnDystropy->setText("异常行为");
    ToolBtnDystropy->setVisible(true);

    ToolBtnTrafficAccident->setIcon(QIcon(":/images/TrafficAccident.png"));
    ToolBtnTrafficAccident->setIconSize(QSize(32, 32));
    ToolBtnTrafficAccident->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ToolBtnTrafficAccident->setText("交通事故");
    ToolBtnTrafficAccident->setVisible(true);

    gridLayout->addWidget(ToolBtnLeave, 0, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnAreaIntrusion, 0, 1, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnVehicleRetrograde, 1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnTrafficFlow, 1, 1 ,Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnTarget, 2, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnPopulation, 2, 1, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnTunnelPedestrian, 3, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnTurnLane, 3, 1, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnIllegalPark, 4, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnVehicleSpeed, 4, 1, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnDystropy, 5, 0, Qt::AlignHCenter);
    gridLayout->addWidget(ToolBtnTrafficAccident, 5, 1, Qt::AlignHCenter);
    gridLayout->setRowStretch(0,0);
    gridLayout->setRowStretch(1,1);

    area->setLayout(gridLayout);
}

void MainWindow::initConnect()
{
    connect(actAbout,&QAction::triggered,[&](){
        QMessageBox::about(this, "智能视频监控系统", "智能视频监控系统 版本："+qApp->applicationVersion());
    });

    connect(actUserManual,&QAction::triggered,[&](){
        QDesktopServices::openUrl(QUrl(QDir::currentPath() + "/li.pdf", QUrl::TolerantMode));
    });

    connect(ToolBtnLeave, &QToolButton::clicked, this, &MainWindow::SlotLeave);
    connect(ToolBtnVehicleRetrograde, &QToolButton::clicked, this, &MainWindow::SlotVehicleRetrograde);
}

void MainWindow::initData()
{
    ProcessDeviate = new QProcess(this);
    ReadSetting();
}

void MainWindow::ReadSetting()
{
    QSettings settings("LPJ", "GUI");
    QPoint pos = settings.value("pos", QVariant(QPoint(20, 20))).toPoint();
    QSize size = settings.value("size", QVariant(QSize(1300, 1000))).toSize();
    this->resize(size);
    this->move(pos);
}

void MainWindow::WriteSetting()
{
    QSettings setting("LPJ", "GUI");
    setting.setValue("pos", this->pos());
    setting.setValue("size", this->size());
}
