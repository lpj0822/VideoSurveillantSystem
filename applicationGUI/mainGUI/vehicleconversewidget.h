#ifndef VEHICLECONVERSEWIDGET_H
#define VEHICLECONVERSEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollArea>
#include <QSettings>
#include <QDockWidget>
#include <QDebug>
#include <QStackedWidget>
#include <QSlider>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QTreeWidget>
#include <QFileDialog>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QProcess>
#include <QToolBar>
#include "expand/wnavigation.h"

class VehicleConverseWidget : public QWidget
{
    Q_OBJECT

public:
    VehicleConverseWidget(QWidget *parent = 0);
    ~VehicleConverseWidget();

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void CentralIndexChanged(int index);

private:
    void initData();
    void initUI();
    void initConnect();

private:
    WNavigation *navigation;
    QStackedWidget *CentralStackedWidget;
};

#endif // VEHICLECONVERSEWIDGET_H
