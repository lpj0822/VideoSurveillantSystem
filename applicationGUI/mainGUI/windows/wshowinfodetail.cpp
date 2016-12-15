#include "wshowinfodetail.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>

#pragma execution_character_set("utf-8")

using namespace QtAV;

WShowInfoDetail::WShowInfoDetail(QWidget *parent) : QDialog(parent),
    m_unit(1000)
{
    initData();
    initUI();
    initConnect();
}

WShowInfoDetail::~WShowInfoDetail()
{
    ;
}

void WShowInfoDetail::initData()
{
    QtAV::Widgets::registerRenderers();
    QtAV::setLogLevel(LogOff);
    Renderer = new GLWidgetRenderer2();
    if (!Renderer->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
        return;
    }
    timeSlider = new QSlider();
    timeSlider->setOrientation(Qt::Horizontal);
    timeSlider->setTickPosition(QSlider::TicksBothSides);

    Player = new AVPlayer(this);
    Player->setRenderer(Renderer);

    lbImage = new QLabel();

    model = new QStandardItemModel();
    model->setColumnCount(2);
    model->setItem(0,0,new QStandardItem("事件时间"));
    model->setItem(1,0,new QStandardItem("事件区域"));
    model->setItem(2,0,new QStandardItem("监控点"));

    table = new QTableView();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setModel(model);
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setMaximumWidth(300);

    btnJudgeSure = new QPushButton("报警属实");
    btnFalseAlarm = new QPushButton("误报");

    this->resize(1200,800);
}

void WShowInfoDetail::initUI()
{
    QHBoxLayout *btnlayout = new QHBoxLayout();
    btnlayout->addWidget(btnJudgeSure);
    btnlayout->addWidget(btnFalseAlarm);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(table);
    rightLayout->addLayout(btnlayout);

    leftLayout = new QVBoxLayout();
    leftLayout->addWidget(Renderer->widget());
    leftLayout->addWidget(timeSlider);
    leftLayout->setStretch(0,1);
    leftLayout->setStretch(1,0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    this->setLayout(mainLayout);
}

void WShowInfoDetail::initConnect()
{
    connect(Player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(Player, SIGNAL(started()), SLOT(updateSlider()));
    connect(timeSlider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(timeSlider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));
    connect(btnJudgeSure,&QPushButton::clicked,
            [&](){
        emit signal_JudgeSure(modelIndex);
        this->close();
    });

    connect(btnFalseAlarm,&QPushButton::clicked,
            [&](){
        emit signal_FalseAlarm(modelIndex);
        this->close();
    });
}

void WShowInfoDetail::playVideo(const QString &fileName)
{
    Player->play(fileName);
}

void WShowInfoDetail::play(const QString &fileName,const QString &fileType)
{
    if(fileType=="Image"){
        qDebug()<<"image";
        lbImage->setPixmap(QPixmap::fromImage(QImage(fileName)));
        lbImage->show();
        Renderer->widget()->hide();
        timeSlider->hide();
        leftLayout->insertWidget(0,lbImage);
    }
    else if (fileType=="Video") {
        qDebug()<<"video";
        lbImage->hide();
        Renderer->widget()->show();
        timeSlider->show();
        leftLayout->insertWidget(0,Renderer->widget());
        Player->play(fileName);
    }
}

void WShowInfoDetail::setEventTime(const QString &time)
{
    model->setItem(0,1,new QStandardItem(time));
}

void WShowInfoDetail::setArea(const QString &name)
{
    model->setItem(1,1,new QStandardItem(name));
}

void WShowInfoDetail::setCamera(const QString &name)
{
    model->setItem(2,1,new QStandardItem(name));
}

void WShowInfoDetail::updateSlider()
{
    updateSlider(Player->position());
}

void WShowInfoDetail::updateSlider(qint64 value)
{
    timeSlider->setRange(0, int(Player->duration()/m_unit));
    timeSlider->setValue(int(value/m_unit));
}

void WShowInfoDetail::seekBySlider(int value)
{
    if (!Player->isPlaying())
        return;
    Player->seek(qint64(value*1000));
}

void WShowInfoDetail::seekBySlider()
{
    seekBySlider(timeSlider->value());
}

void WShowInfoDetail::updateSliderUnit()
{
    m_unit = Player->notifyInterval();
    updateSlider();
}


void WShowInfoDetail::closeEvent(QCloseEvent *)
{
    if (Player)
        Player->stop();
}

void WShowInfoDetail::setEnablJudgeSured(bool JudgeSured)
{
    if(!JudgeSured){
        btnJudgeSure->hide();
        btnFalseAlarm->hide();
    }
    else {
        btnJudgeSure->show();
        btnFalseAlarm->show();
    }
}

void WShowInfoDetail::setTitle(const QString &title)
{
    this->setWindowTitle(title);
}

void WShowInfoDetail::setModelIndex(const QModelIndex &index)
{
    modelIndex = index;
}
