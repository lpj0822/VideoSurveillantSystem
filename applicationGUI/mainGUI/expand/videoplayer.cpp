#include "videoplayer.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>
#include "../expand/myhelper.h"

#pragma execution_character_set("utf-8")

using namespace QtAV;

VideoPlayer::VideoPlayer(QWidget *parent) : QDialog(parent),
    m_unit(1000),
    dplayset(nullptr)
{
    initData();
    initUI();
    initConnect();
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::initData()
{
    QtAwesome* awesome = MyHelper::getAwesome();
    QtAV::Widgets::registerRenderers();
    QtAV::setLogLevel(LogOff);
    Renderer = new GLWidgetRenderer2(this);
    if (!Renderer->widget()) {
        QMessageBox::warning(this, QString("播放错误"), tr("渲染器出错"));
        return;
    }
    Player = new AVPlayer(this);
    Player->setRenderer(Renderer);

    //above
    AboveControl = new QFrame(this);
    timeSlider = new Slider();
    timeSlider->setOrientation(Qt::Horizontal);
    timeSlider->setMouseTracking(true);

    tbtnVolume = new QToolButton();
    tbtnVolume->setCheckable(true);
    tbtnVolume->setChecked(false);
    tbtnVolume->setIcon(QIcon(":/images/volume_up.png"));

    volumeSlider = new Slider();
    volumeSlider->setMaximumWidth(200);
    volumeSlider->setOrientation(Qt::Horizontal);
    volumeSlider->setMouseTracking(true);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(volumeSliderMax);
    volumeSlider->setValue(int(1.0/volumeInterval*qreal(volumeSliderMax)/100.0));

    //bottom
    BottomControl = new QFrame(this);
    tbtnPlay = new QToolButton();
    QVariantMap myblue;
    myblue.insert("color",QColor("#2EA4FF") );
    tbtnPlay->setIcon(awesome->icon(fa::pause,myblue));
    lbTimeCurrent  = new QLabel();
    lbTimeEnd  = new QLabel();
    tbtnSet = new QToolButton();
    tbtnSet->setIcon(awesome->icon(fa::gear));
    initEQEngine();

    this->resize(1200,800);
}

void VideoPlayer::initUI()
{
    QHBoxLayout *aboveLayout = new QHBoxLayout();
    aboveLayout->setMargin(0);
    aboveLayout->addWidget(timeSlider);
    aboveLayout->addWidget(tbtnVolume);
    aboveLayout->addWidget(volumeSlider);
    AboveControl->setLayout(aboveLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->addStretch(0);
    bottomLayout->addWidget(tbtnPlay);
    bottomLayout->addWidget(lbTimeCurrent);
    bottomLayout->addWidget(lbTimeEnd);
    bottomLayout->addStretch(0);
    bottomLayout->addWidget(tbtnSet);
    BottomControl->setLayout(bottomLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(Renderer->widget());
    mainLayout->addWidget(AboveControl);
    mainLayout->addWidget(BottomControl);
    mainLayout->setStretch(0,1);
    mainLayout->setStretch(1,0);
    mainLayout->setStretch(2,0);
    this->setLayout(mainLayout);
}

void VideoPlayer::initConnect()
{
    connect(Player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(Player, SIGNAL(started()), SLOT(updateSlider()));
    connect(timeSlider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(timeSlider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));

    connect(Player,&AVPlayer::started,[&]{
        lbTimeEnd->setText("/ "+QTime(0, 0, 0).addMSecs(Player->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    });

    connect(Player,&AVPlayer::positionChanged,[&](qint64 pos){
        lbTimeCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
    });

    connect(tbtnPlay,&QToolButton::clicked,[&]{
        QtAwesome* awesome = MyHelper::getAwesome();
        QVariantMap myblue;
        myblue.insert("color",QColor("#2EA4FF") );
        if(Player->isPlaying()){
            if(Player->isPaused()){
                Player->pause(false);
                tbtnPlay->setIcon(awesome->icon(fa::pause,myblue));
            }
            else{
                Player->pause(true);
                tbtnPlay->setIcon(awesome->icon(fa::play,myblue));
            }
        }
    });

    connect(tbtnVolume,&QToolButton::clicked,[&]{
        AudioOutput *ao = Player ? Player->audio() : 0;
        if (ao){
            if(tbtnVolume->isChecked()){
                volume = ao->volume();
                ao->setVolume(0);
                tbtnVolume->setIcon(QIcon(":/images/volume_off.png"));
            }
            else {
                ao->setVolume(volume);
                tbtnVolume->setIcon(QIcon(":/images/volume_up.png"));
            }
        }
    });

    connect(volumeSlider, SIGNAL(sliderPressed()), SLOT(setVolume()));
    connect(volumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume()));

    connect(tbtnSet,&QToolButton::clicked,[&](){
        if(!dplayset){
            dplayset = new DPlayerSet(this);
            connect(dplayset, SIGNAL(brightnessChanged(int)), this, SLOT(onBrightnessChanged(int)));
            connect(dplayset, SIGNAL(contrastChanged(int)), this, SLOT(onContrastChanged(int)));
            connect(dplayset, SIGNAL(hueChanegd(int)), this, SLOT(onHueChanged(int)));
            connect(dplayset, SIGNAL(saturationChanged(int)), this, SLOT(onSaturationChanged(int)));
        }
        dplayset->exec();
    });
}

void VideoPlayer::playVideo(const QString &fileName)
{
    Player->play(fileName);
}

void VideoPlayer::updateSlider()
{
    updateSlider(Player->position());
}

void VideoPlayer::updateSlider(qint64 value)
{
    timeSlider->setRange(0, int(Player->duration()/m_unit));
    timeSlider->setValue(int(value/m_unit));
}

void VideoPlayer::seekBySlider(int value)
{
    if (!Player->isPlaying())
        return;
    Player->seek(qint64(value*1000));
}

void VideoPlayer::seekBySlider()
{
    qDebug()<<"moved";
    seekBySlider(timeSlider->value());
    if(!Player->isPlaying()){
        Player->play();
    }
}

void VideoPlayer::updateSliderUnit()
{
    m_unit = Player->notifyInterval();
    updateSlider();
}

void VideoPlayer::setTitle(const QString &title)
{
    this->setWindowTitle(title);
}

void VideoPlayer::setVolume()
{
    AudioOutput *ao = Player ? Player->audio() : 0;
    qreal v = qreal(volumeSlider->value())*volumeInterval;
    if (ao) {
        if (qAbs(int(ao->volume()/volumeInterval) - volumeSlider->value()) >= int(0.1/volumeInterval)) {
            ao->setVolume(v);
        }
    }
    volumeSlider->setToolTip(QString::number(v));
}

void VideoPlayer::initEQEngine()
{
    VideoRenderer *vo = Player->renderer();
    vo->forcePreferredPixelFormat(false);
}


void VideoPlayer::onBrightnessChanged(int b)
{
    VideoRenderer *vo = Player->renderer();

    if(vo->setBrightness((qreal)b/100)) {
        Player->setBrightness(0);
    }
}

void VideoPlayer::onContrastChanged(int c)
{
    VideoRenderer *vo = Player->renderer();

    if(vo->setContrast((qreal)c/100)) {
        Player->setContrast(0);
    }
}

void VideoPlayer::onHueChanged(int h)
{
    VideoRenderer *vo = Player->renderer();
    if(vo->setHue((qreal)h/100)) {
        Player->setHue(0);
    }
}

void VideoPlayer::onSaturationChanged(int s)
{
    VideoRenderer *vo = Player->renderer();

    if(vo->setSaturation((qreal)s/100)) {
        Player->setSaturation(0);
    }
}

