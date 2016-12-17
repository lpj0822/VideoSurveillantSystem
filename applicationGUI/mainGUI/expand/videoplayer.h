#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QSlider>
#include <QtAV>
#include <QtAVWidgets>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include "Slider.h"
#include "dplayerset.h"

const int volumeSliderMax = 100;
const qreal volumeInterval = 0.04;

class VideoPlayer : public QDialog
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

    void playVideo(const QString &fileName);
    void setTitle(const QString &title);

signals:

public slots:

protected:

private slots:
    void updateSlider();
    void updateSlider(qint64 value);
    void seekBySlider(int value);
    void seekBySlider();
    void updateSliderUnit();
    void setVolume();
    void onBrightnessChanged(int b);
    void onContrastChanged(int c);
    void onHueChanged(int h);
    void onSaturationChanged(int s);

private:
    void initData();
    void initUI();
    void initConnect();
    void initEQEngine();

private:
    QtAV::AVPlayer *Player;
    QtAV::GLWidgetRenderer2 *Renderer;

    //above control
    QFrame *AboveControl;
    Slider *timeSlider;
    Slider *volumeSlider;
    QToolButton *tbtnVolume;

    //bottom control
    QFrame *BottomControl;
    QToolButton *tbtnPlay;
    QLabel *lbTimeCurrent, *lbTimeEnd;
    QToolButton *tbtnSet;
    DPlayerSet *dplayset;
    int m_unit;
    int playStart;
    int playEnd;
    qreal volume;
};

#endif // VIDEOPLAYER_H
