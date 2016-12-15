#pragma execution_character_set("utf-8")
#include "wdevicemanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "wserver.h"

WDeviceManager::WDeviceManager(QWidget *parent) : QFrame(parent)
{
    initData();
    initUI();
    initConnect();
}

void WDeviceManager::initData()
{
    tbtnServer = new QToolButton();
    tbtnServer->setText("服务器");
    tbtnServer->setCheckable(true);
    tbtnServer->setMinimumWidth(120);
    tbtnServer->setMinimumHeight(25);
    tbtnGroup.append(tbtnServer);

    stackedWidget = new QStackedWidget();
    WServer *server = new WServer();
    stackedWidget->addWidget(server);
    stackedWidget->setCurrentIndex(0);
}

void WDeviceManager::initUI()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(tbtnServer);
    leftLayout->addStretch(0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(stackedWidget);

    this->setLayout(mainLayout);
}

void WDeviceManager::initConnect()
{
    for(auto btn: tbtnGroup)
    {
        connect(btn, SIGNAL(clicked()), this, SLOT(on_btnClicked()));
    }
}

void WDeviceManager::on_btnClicked()
{
    for(auto btn: tbtnGroup)
    {
        if (btn == sender()){
            btn->setChecked(true);
            stackedWidget->setCurrentIndex(tbtnGroup.indexOf(btn));
        }
        else{
            btn->setChecked(false);
        }
    }
}
