#include "leaveconfig.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#pragma execution_character_set("utf-8")

LeaveConfig::LeaveConfig(QWidget *parent) : QDialog(parent)
{
    initData();
    initUI();
    initConnect();
}

LeaveConfig::~LeaveConfig()
{
}

void LeaveConfig::initData()
{
    editArea  = new EditableLabel();
    btnReConfig = new QPushButton("重新配置");
    btnOK = new QPushButton("确认");
    btnCancel = new QPushButton("取消");

    timeEdit = new QTimeEdit(QTime(0, 5, 0));
    timeEdit->setMaximumTime(QTime(5, 59, 0));
    timeEdit->setMinimumTime(QTime(0, 5, 0));
    timeEdit->setTime(QTime(0, 5, 0));
    timeEdit->setDisplayFormat("HH:mm");
}

void LeaveConfig::initUI()
{
    QHBoxLayout *belowLayout = new QHBoxLayout();
    belowLayout->addStretch(0);
    //belowLayout->addWidget(timeEdit);
    belowLayout->addWidget(btnReConfig);
    belowLayout->addWidget(btnOK);
    belowLayout->addWidget(btnCancel);
    belowLayout->addStretch(0);

    QHBoxLayout *midLayout = new QHBoxLayout();
    midLayout->addStretch(0);
    midLayout->addWidget(new QLabel("超时时间"));
    midLayout->addWidget(timeEdit);
    midLayout->addStretch(0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(editArea);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(belowLayout);
    this->setLayout(mainLayout);
}

void LeaveConfig::initConnect()
{
    connect(btnOK, &QPushButton::clicked, this, &LeaveConfig::on_btnOKClicked);
    connect(btnCancel, &QPushButton::clicked, this, &LeaveConfig::on_btnCancleClicked);
    connect(btnReConfig ,&QPushButton::clicked, this, &LeaveConfig::on_btnReConfigClicked);
}

void LeaveConfig::updatePreview(const QImage &image)
{
    QImage img(image);
    editArea->SetNewQImage(img);
    this->resize(image.width(), image.height()*11/10);
}

int LeaveConfig::getTime()
{
    int time = timeEdit->time().hour() * 60 * 60 + timeEdit->time().minute() * 60 + timeEdit->time().second();
    qDebug() << "time:" << time;
    return time;
}

QList<QPolygonF> LeaveConfig::getPolygon()
{
    return editArea->GetPoints();
}

void LeaveConfig::on_btnOKClicked()
{
    this->accept();
}

void LeaveConfig::on_btnCancleClicked()
{
    this->close();
}

void LeaveConfig::on_btnReConfigClicked()
{
    editArea->ClearPoints();
}
