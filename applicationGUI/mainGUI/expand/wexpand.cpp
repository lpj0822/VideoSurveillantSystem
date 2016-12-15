#include "wexpand.h"
#include <QVBoxLayout>
#include <QDebug>
#include "myhelper.h"

WExpand::WExpand(QWidget *parent) : QFrame(parent),
    expanded(true)
{
    QtAwesome* awesome = MyHelper::getAwesome();
    btn = new QToolButton(this);
    btn->setObjectName("WExpand_btn");
    btn->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    btn->setMaximumWidth(10);
    btn->setText(QChar(fa::angleleft));
    btn->setFont(awesome->font(16));

    connect(btn,&QToolButton::clicked,[&](){
        qDebug()<<"clicked";
        if(expanded){
            btn->setText(QChar(fa::angleright));
        }
        else {
            btn->setText(QChar(fa::angleleft));
        }
        expanded=!expanded;
        emit signalStatusChangeed(expanded);
    });

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->addStretch(1);
    mainLayout->addWidget(btn);
    mainLayout->addStretch(1);
    this->setLayout(mainLayout);
    this->setMinimumWidth(3);
}
