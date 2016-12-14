#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include "applicationGUI/simpleGUI/leavedetectcontrolwindow.h"
#include "applicationGUI/simpleGUI/intrudedetectcontrolwindow.h"
#include "applicationGUI/simpleGUI/vehicleconversecontrolwindow.h"
#include "applicationGUI/simpleGUI/vehiclecountingcontrolwindow.h"

class SimpleControlWindow : public QWidget
{
    Q_OBJECT

public:
    SimpleControlWindow(QWidget *parent = 0);
    ~SimpleControlWindow();

signals:

public slots:
    void slotOK();//点击确定

private:

    QVBoxLayout *mainLayout;//布局
    QTabWidget *tabweight;//tab
    QDialogButtonBox *buttonBox;

    LeaveDetectControlWindow *leaveDetectWindow;//离岗检测
    IntrudeDetectControlWindow *intrudeDetectWindow;//入侵检测
    VehicleConverseControlWindow *vehicleConverseWindow;//车辆逆行检测
    VehicleCountingControlWindow *vehicleCountingWindow;//车流量统计

    void init();
    void initUI();
    void initConnect();
};
#endif // CONTROLWINDOW_H
