#include "vehicleconversewidget.h"
#include <QApplication>
#include "windows/wmainpreview.h"
#include "windows/weventsearch.h"
#include "windows/wdevicemanager.h"
#include "localResource/wlocalresource.h"
#include "expand/myhelper.h"
#include <QMessageBox>

#pragma execution_character_set("utf-8")

VehicleConverseWidget::VehicleConverseWidget(QWidget *parent)
    : QWidget(parent)
{
    initData();
    initUI();
    initConnect();
}

VehicleConverseWidget::~VehicleConverseWidget()
{
}

void VehicleConverseWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}

void VehicleConverseWidget::initData()
{
    WMainPreview *mainPreview = new WMainPreview();
    WEventSearch *eventSearch = new WEventSearch();
    WLocalResource *localResource = new WLocalResource(3);
    WDeviceManager *deviceManager = new WDeviceManager();

    CentralStackedWidget = new QStackedWidget(this);
    CentralStackedWidget->addWidget(mainPreview);
    CentralStackedWidget->addWidget(eventSearch);
    CentralStackedWidget->addWidget(localResource);
    CentralStackedWidget->addWidget(deviceManager);
    CentralStackedWidget->setCurrentIndex(0);
}

void VehicleConverseWidget::initUI()
{
    QtAwesome* awesome = MyHelper::getAwesome();

    navigation = new WNavigation(this);
    navigation->AddBtn("主预览区", awesome->icon(fa::play));
    navigation->AddBtn("信息搜索", awesome->icon(fa::search));
    navigation->AddBtn("本地资源", awesome->icon(fa::film));
    navigation->AddBtn("设备管理", awesome->icon(fa::server));

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(navigation);
    mainLayout->addWidget(CentralStackedWidget);

    this->setLayout(mainLayout);
}

void VehicleConverseWidget::initConnect()
{
    connect(navigation, &WNavigation::signalBtnClicked, this, &VehicleConverseWidget::CentralIndexChanged);
}

void VehicleConverseWidget::CentralIndexChanged(int index)
{
    CentralStackedWidget->setCurrentIndex(index);
}
