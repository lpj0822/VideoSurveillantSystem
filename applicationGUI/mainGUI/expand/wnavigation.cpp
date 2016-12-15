#include "wnavigation.h"
#include <QDebug>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QApplication>
#include "mainwindow.h"

#pragma execution_character_set("utf-8")

WNavigation::WNavigation(QWidget *parent) : QFrame(parent),
    btnWidth(120)
{
    mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addStretch();
    this->setLayout(mainLayout);
    this->setFixedWidth(btnWidth);
    this->setStyleSheet(".WNavigation{background-color: #201F1F;border: 1px solid #3A3939;}");
}

WNavigation::~WNavigation()
{
    qDebug()<<"WNavigation::~WNavigation()";
}

void WNavigation::AddBtn(const QString &str,const QIcon &icon)
{
    QToolButton *btn = new QToolButton(this);
    btn->setText(str);
    btn->setIcon(icon);
    btn->setIconSize(QSize(32,32));
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setMinimumWidth(btnWidth);
    btn->setCheckable(true);
    btn->setFocusPolicy(Qt::NoFocus);
    connect(btn,&QToolButton::clicked,this,&WNavigation::slotBtnClicked);
    mainLayout->insertWidget(btnGroup.size(),btn);
    btnGroup.append(btn);
}

void WNavigation::slotBtnClicked()
{
    MainWindow *mainwindow = qApp->property("MainWindow").value<MainWindow *>();
    for(auto i=0;i<btnGroup.size();i++){
        if(btnGroup.at(i)==sender()){
            emit signalBtnClicked(i);
            mainwindow->addLogMsg(btnGroup.at(i)->text());
            btnGroup.at(i)->setChecked(true);
        }
        else{
            btnGroup.at(i)->setChecked(false);
        }
    }
}
