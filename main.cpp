#pragma execution_character_set("utf-8")
#include <QTextCodec>
#include <QApplication>
#include <QDesktopWidget>
#include "controlwindow.h"
#include "vehiclecountingcontrolwindow.h"
#include "vehicleconversecontrolwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //a.setApplicationName("智能监控系统");
    VehicleCountingControlWindow w;
    w.show();
    w.move((QApplication::desktop()->width() - w.width())/2,(QApplication::desktop()->height() - w.height())/2);

    return a.exec();
}
