#include "dplayerset.h"
#include <QGridLayout>
#include <QLabel>

#pragma execution_character_set("utf-8")

DPlayerSet::DPlayerSet(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("参数调节");
    mpBSlider = new QSlider(Qt::Horizontal);
    mpBSlider->setTickInterval(2);
    mpBSlider->setRange(-100, 100);
    mpBSlider->setValue(0);
    mpCSlider = new QSlider(Qt::Horizontal);
    mpCSlider->setTickInterval(2);
    mpCSlider->setRange(-100, 100);
    mpCSlider->setValue(0);
    mpHSlider = new QSlider(Qt::Horizontal);
    mpHSlider->setTickInterval(2);
    mpHSlider->setRange(-100, 100);
    mpHSlider->setValue(0);
    mpSSlider = new QSlider(Qt::Horizontal);
    mpSSlider->setTickInterval(2);
    mpSSlider->setRange(-100, 100);
    mpSSlider->setValue(0);

    btnReset = new QPushButton("恢复默认");

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(new QLabel("亮度"),0,0);
    mainLayout->addWidget(mpBSlider,0,1);
    mainLayout->addWidget(new QLabel("对比度"),1,0);
    mainLayout->addWidget(mpCSlider,1,1);
    mainLayout->addWidget(new QLabel("色调"),2,0);
    mainLayout->addWidget(mpHSlider,2,1);
    mainLayout->addWidget(new QLabel("饱和度"),3,0);
    mainLayout->addWidget(mpSSlider,3,1);
    mainLayout->addWidget(btnReset,4,0,1,2);
    this->setLayout(mainLayout);

    connect(mpBSlider, SIGNAL(valueChanged(int)), SIGNAL(brightnessChanged(int)));
    connect(mpCSlider, SIGNAL(valueChanged(int)), SIGNAL(contrastChanged(int)));
    connect(mpHSlider, SIGNAL(valueChanged(int)), SIGNAL(hueChanegd(int)));
    connect(mpSSlider, SIGNAL(valueChanged(int)), SIGNAL(saturationChanged(int)));

    connect(btnReset,&QPushButton::clicked,[&]{
        mpBSlider->setValue(0);
        mpCSlider->setValue(0);
        mpHSlider->setValue(0);
        mpSSlider->setValue(0);
    });
}
