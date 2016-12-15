#include "wmainpreview.h"
#include <QHBoxLayout>
#include <QSettings>
#include <QHeaderView>
#include "../expand/myhelper.h"

#pragma execution_character_set("utf-8")

WMainPreview::WMainPreview(QWidget *parent) : QFrame(parent)
{
    initData();
    InitLocalConfig();
    initUI();
    initConnect();
}

void WMainPreview::initData()
{
    QtAwesome* awesome = MyHelper::getAwesome();
    treeArea = new WAreaTree();

    toolBox = new QToolBox();
    toolBox->setFixedWidth(250);

    tbtnView_1 = new QToolButton();
    tbtnView_1->setIcon(awesome->icon(fa::thlist));
    tbtnView_1->setText("1视图");
    tbtnView_1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbtnView_1->setIconSize(QSize(32,32));
    tbtnView_4 = new QToolButton();
    tbtnView_4->setIcon(awesome->icon(fa::thlarge));
    tbtnView_4->setText("4视图");
    tbtnView_4->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbtnView_4->setIconSize(QSize(32,32));
    tbtnView_9 = new QToolButton();
    tbtnView_9->setIcon(awesome->icon(fa::th));
    tbtnView_9->setText("9视图");
    tbtnView_9->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbtnView_9->setIconSize(QSize(32,32));
    tbtnView_16 = new QToolButton();
    tbtnView_16->setIcon(awesome->icon(fa::thlarge));
    tbtnView_16->setText("16视图");
    tbtnView_16->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbtnView_16->setIconSize(QSize(32,32));

    QWidget *viewWidget = new QWidget();
    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(tbtnView_1);
    viewLayout->addWidget(tbtnView_4);
    viewLayout->addWidget(tbtnView_9);
    viewLayout->addWidget(tbtnView_16);
    viewLayout->addStretch(0);
    viewWidget->setLayout(viewLayout);

    toolBox->addItem(treeArea,"区域和监控点");
    toolBox->addItem(viewWidget,"视图观看");
    toolBox->addItem(new QWidget,"云台控制");

    palyWindow = new WPlayWindow();
}

void WMainPreview::InitLocalConfig()
{
    ;
}

void WMainPreview::initConnect()
{
    connect(tbtnView_1,&QToolButton::clicked,[&](){
        palyWindow->setViewType(WPlayWindow::ViewType_1);
    });

    connect(tbtnView_4,&QToolButton::clicked,[&](){
        palyWindow->setViewType(WPlayWindow::ViewType_4);
    });

    connect(tbtnView_9,&QToolButton::clicked,[&](){
        palyWindow->setViewType(WPlayWindow::ViewType_9);
    });

    connect(tbtnView_16,&QToolButton::clicked,[&](){
        palyWindow->setViewType(WPlayWindow::ViewType_16);
    });
}

void WMainPreview::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(toolBox);
    mainLayout->addWidget(palyWindow);

    this->setLayout(mainLayout);
}
