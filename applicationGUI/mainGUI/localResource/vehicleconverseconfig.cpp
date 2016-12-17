#include "vehicleconverseconfig.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#pragma execution_character_set("utf-8")

VehicleConverseConfig::VehicleConverseConfig(QWidget *parent) : QDialog(parent)
{
    initData();
    initUI();
    initConnect();
}

VehicleConverseConfig::~VehicleConverseConfig()
{
}

void VehicleConverseConfig::initData()
{
    editArea  = new EditableLabel();
    btnReConfig = new QPushButton("重新配置");
    btnOK = new QPushButton("确认");
    btnCancel = new QPushButton("取消");

    cbbDirection = new QComboBox(this);
    cbbDirection->addItem("上");
    cbbDirection->addItem("下");
    cbbDirection->addItem("左");
    cbbDirection->addItem("右");
}

void VehicleConverseConfig::initUI()
{
    QHBoxLayout *belowLayout = new QHBoxLayout();
    belowLayout->addStretch(0);
    belowLayout->addWidget(btnReConfig);
    belowLayout->addWidget(btnOK);
    belowLayout->addWidget(btnCancel);
    belowLayout->addStretch(0);

    QHBoxLayout *midLayout = new QHBoxLayout();
    midLayout->addStretch(0);
    midLayout->addWidget(new QLabel("方向选择"));
    midLayout->addWidget(cbbDirection);
    midLayout->addStretch(0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(editArea);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(belowLayout);
    this->setLayout(mainLayout);
}

void VehicleConverseConfig::initConnect()
{
    connect(btnOK, &QPushButton::clicked, this, &VehicleConverseConfig::on_btnOKClicked);
    connect(btnCancel, &QPushButton::clicked, this, &VehicleConverseConfig::on_btnCancleClicked);
    connect(btnReConfig, &QPushButton::clicked, this, &VehicleConverseConfig::on_btnReConfigClicked);
}

void VehicleConverseConfig::updatePreview(const QImage &image)
{
    QImage img(image);
    editArea->SetNewQImage(img);
    this->resize(image.width(), image.height()*11/10);
}

int VehicleConverseConfig::getDirection()
{
    int direction;
    QString str = cbbDirection->currentText();
    if(str=="上"){
        direction = 1;
    }
    else if (str=="下") {
        direction = 2;
    }
    else if (str=="左") {
        direction = 3;
    }
    else if (str=="右") {
        direction = 4;
    }
    else{
        direction = 0;
    }

    return direction;
}

QList<QPolygonF> VehicleConverseConfig::getPolygon()
{
    return editArea->GetPoints();
}

void VehicleConverseConfig::on_btnOKClicked()
{
    qDebug()<<getDirection();
    this->accept();
}

void VehicleConverseConfig::on_btnCancleClicked()
{
    this->close();
}

void VehicleConverseConfig::on_btnReConfigClicked()
{
    editArea->ClearPoints();
}
