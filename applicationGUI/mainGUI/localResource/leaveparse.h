#ifndef LEAVEPARSE_H
#define LEAVEPARSE_H

#include <QDialog>
#include <QtAV>
#include <QtAVWidgets>
#include <QSlider>
#include <QTableView>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QLabel>
#include <QListWidget>
#include "../windows/wtitletableview.h"
#include "videoAnalysis/leaveshowvideowindow.h"

class LeaveParse : public QDialog
{
    Q_OBJECT
public:
    explicit LeaveParse(QWidget *parent = 0);
    ~LeaveParse();
    void playVideo(const QString &fileName);

signals:
    void signal_NewPreviewInfo();

public slots:
    void slotVideoMessage(bool isVideoOpen);
    void slotLeaveMessage(int number);
    void slotSaveVideoMessage(QString savePath);

protected:

private slots:
    void on_BeginConfig();
    void on_ShowInfoDetails(QListWidgetItem *item);

private:
    void initData();
    void initDetectData();
    void initUI();
    void initConnect();

private:

    LeaveShowVideoWindow *lbPlay;
    QPushButton *btnConfig;
    QPushButton *btnParse;
    QListWidget *listWidget;

    QString path;
    QImage saveImg;
};

#endif // LEAVEPARSE_H
