#ifndef WDEVICEMANAGER_H
#define WDEVICEMANAGER_H

#include <QFrame>
#include <QToolButton>
#include <QStackedWidget>

class WDeviceManager : public QFrame
{
    Q_OBJECT
public:
    explicit WDeviceManager(QWidget *parent = 0);

signals:

public slots:

private slots:
    void on_btnClicked();

private:
    void initData();
    void initUI();
    void initConnect();

private:
    QToolButton *tbtnServer;

    QStackedWidget *stackedWidget;

    QList<QToolButton *> tbtnGroup;
};

#endif // WDEVICEMANAGER_H
