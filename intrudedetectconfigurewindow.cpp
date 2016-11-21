#pragma execution_character_set("utf-8")
#include "intrudedetectconfigurewindow.h"
#include <QVector>
#include <QPoint>
#include <QMessageBox>
#include <QEvent>
#include <QDebug>

IntrudeDetectConfigureWindow::IntrudeDetectConfigureWindow(QImage image, QWidget *parent) : QDialog(parent)
{
    this->pixmap = QPixmap::fromImage(image);
    initUI();
    initData();
    initConnect();
}

IntrudeDetectConfigureWindow::~IntrudeDetectConfigureWindow()
{
    drawArea->deleteLater();
}

void IntrudeDetectConfigureWindow::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        break;
    case QEvent::Close:
        reject();
        break;
    default:
       break;
    }
}

//得到检测区域
QList<QPolygonF> IntrudeDetectConfigureWindow::getDetectArea()
{
    return area;
}

void IntrudeDetectConfigureWindow::slotOk()
{
    QString message = "";
    QVector<QPoint> tempPolygon = drawArea->getPolygon();
    QPolygonF tempData(tempPolygon);
    if (tempPolygon.size() <= 2)
    {
        message += "划定检测区域不正确!";
    }
    area.push_back(tempData);
    if (message.trimmed() == "")
    {
        accept();
    }
    else
    {
        QMessageBox::information(this, tr("配置参数不合法"), message);
    }

}

void IntrudeDetectConfigureWindow::slotRest()
{
    drawArea->restImage(pixmap);
    area.clear();
}

//初始化UI
void IntrudeDetectConfigureWindow::initUI()
{
    mainLayout = new QVBoxLayout();

    drawArea = new DrawingWindow(pixmap);
    drawArea->setMouseTracking(true);
    this->widthWindow=drawArea->width() + 20;
    this->heightWindow=drawArea->height() + 60;

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->setSpacing(widthWindow / 6);
    okButton = new QPushButton(tr("确定"));
    cancelButton = new QPushButton(tr("取消"));
    restButton = new QPushButton(tr("重绘"));//重绘
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(restButton);

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(drawArea);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);

    this->setMaximumSize(widthWindow, heightWindow);
    this->setMinimumSize(widthWindow, heightWindow);
    this->setWindowTitle(tr("入侵检测系统参数配置"));
}

//初始化数据
void IntrudeDetectConfigureWindow::initData()
{
    area.clear();
}

//事件连接
void IntrudeDetectConfigureWindow::initConnect()
{
    connect(okButton, &QPushButton::clicked, this, &IntrudeDetectConfigureWindow::slotOk);
    connect(cancelButton, &QPushButton::clicked, this, &IntrudeDetectConfigureWindow::reject);
    connect(restButton, &QPushButton::clicked, this, &IntrudeDetectConfigureWindow::slotRest);
}

