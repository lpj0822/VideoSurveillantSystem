#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollArea>
#include <QDockWidget>
#include <QDebug>
#include <QStackedWidget>
#include <QSlider>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QResizeEvent>
#include <QToolBar>
#include <QTreeWidget>
#include <QFileDialog>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QSettings>
#include <QProcess>
#include "applicationGUI/mainGUI/expand/customanimation.h"
#include "applicationGUI/mainGUI/mainwidget.h"
#include "applicationGUI/mainGUI/vehicleconversewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addLogMsg(const QString &str);

protected:
    void closeEvent(QCloseEvent *event);//virtual
    void resizeEvent(QResizeEvent *e);

private slots:

    void SlotLeave();
    void SlotVehicleRetrograde();

private:

    QGridLayout *MainLayout;

    //Left
    QTabWidget *LeftTab;
    //left tool Btn
    QToolButton *ToolBtnLeave;
    QToolButton *ToolBtnAreaIntrusion;
    QToolButton *ToolBtnVehicleRetrograde;
    QToolButton *ToolBtnTrafficFlow;
    QToolButton *ToolBtnTarget;
    QToolButton *ToolBtnPopulation;
    QToolButton *ToolBtnTunnelPedestrian;
    QToolButton *ToolBtnTurnLane;
    QToolButton *ToolBtnIllegalPark;
    QToolButton *ToolBtnVehicleSpeed;
    QToolButton *ToolBtnDystropy;
    QToolButton *ToolBtnTrafficAccident;

    //Right
    QDockWidget *RightDock;
    QTabWidget *RightTab;
    QPlainTextEdit *TextEvent;

    //main
    QStackedWidget *CentralWidget;

    //centre show widget
    QTabWidget *mainCentralTab;
    QWidget *MidPlayVideoWidget;
    int PlayVideoViewIndex;
    QWidget *cenitem;
    CustomAnimation *customAnimation;
    QLabel *label;

    MainWidget *mainWidget;

    //MenuBar
    QMenuBar *AboveMenuBar;
    QMenu *FunctionMenu;

    //action
    QAction *actAbout;
    QAction *actUserManual;
    int leftTabminmumwidth;
    bool leftTabXxpanded;

    VehicleConverseWidget *vehicleConverseWidget;

    //process
    QProcess *ProcessDeviate;

    //Init function
    void InitMenuBar();
    void InitGui();
    void InitMain();
    void InitLeft();
    void InitCentral();
    void InitRight();
    void InitAction();
    void initStackCenterWidget();
    void initConnect();
    void InitLeftToolBtn(QFrame *TabFunc);
    void initData();

    //setting
    void ReadSetting();
    void WriteSetting();
};

#endif // MAINWINDOW_H
