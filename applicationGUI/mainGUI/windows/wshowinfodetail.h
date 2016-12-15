#ifndef WSHOWINFODETAIL_H
#define WSHOWINFODETAIL_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QSlider>
#include <QtAV>
#include <QtAVWidgets>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>

class WShowInfoDetail : public QDialog
{
    Q_OBJECT
public:
    explicit WShowInfoDetail(QWidget *parent = 0);
    ~WShowInfoDetail();
    void playVideo(const QString &fileName);
    void play(const QString &fileName,const QString &fileType);
    void setEventTime(const QString &time);
    void setArea(const QString &name);
    void setCamera(const QString &name);
    void setEnablJudgeSured(bool JudgeSured);
    void setTitle(const QString &title);
    void setModelIndex(const QModelIndex &index);

signals:
    void signalClose();
    void signal_JudgeSure(const QModelIndex &index);
    void signal_FalseAlarm(const QModelIndex &index);


public slots:

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void updateSlider();
    void updateSlider(qint64 value);
    void seekBySlider(int value);
    void seekBySlider();
    void updateSliderUnit();

private:
    void initData();
    void initUI();
    void initConnect();

private:
    QtAV::AVPlayer *Player;
    QtAV::GLWidgetRenderer2 *Renderer;
    QSlider *timeSlider;

    QTableView *table;
    QStandardItemModel *model;

    QPushButton *btnJudgeSure;
    QPushButton *btnFalseAlarm;
    QModelIndex modelIndex;
    int m_unit;

    QLabel *lbImage;
    QVBoxLayout *leftLayout;
};

#endif // WSHOWINFODETAIL_H
