#ifndef VEHICLECONVERSEPARSE_H
#define VEHICLECONVERSEPARSE_H

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
#include "videoAnalysis/vehicleconverseshowvideowindow.h"

class VehicleConverseParse: public QDialog
{
    Q_OBJECT
public:
    explicit VehicleConverseParse(QWidget *parent = 0);
    ~VehicleConverseParse();
    void playVideo(const QString &fileName);

signals:
    void signal_NewPreviewInfo();

public slots:
    void slotVideoMessage(bool isVideoOpen);
    void slotConverseMessage(int number, QString savePath);

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

    VehicleConverseShowVideoWindow *lbPlay;
    QPushButton *btnConfig;
    QPushButton *btnParse;
    QListWidget *listWidget;

    QString path;
};

#endif // VEHICLECONVERSEPARSE_H
