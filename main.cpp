#pragma execution_character_set("utf-8")
#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QThread>
#include "qglobal.h"
#include "simplecontrolwindow.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    app.setApplicationVersion("1.0");
    app.setApplicationName("智能视频监控系统");

    QPixmap pixmap(":/images/sp.png");
    QSplashScreen splash(pixmap);
    splash.show();

    splash.showMessage("Loaded Files", Qt::AlignRight | Qt::AlignTop, Qt::red);
    qApp->processEvents();
    QThread::msleep(4000);

    //load qss file
    QFile file( ":/style/style/style.qss");
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);

    splash.showMessage("connecting......", Qt::AlignRight | Qt::AlignTop, Qt::red);
    QThread::msleep(400);

    SimpleControlWindow w;
    w.show();
    w.move((QApplication::desktop()->width() - w.width())/2, (QApplication::desktop()->height() - w.height())/2);

//    MainWindow w;
//    w.show();
//    QVariant d;//global property
//    d.setValue(&w);
//    app.setProperty("MainWindow",d);

    splash.finish(&w);//splash is finish
    return app.exec();
}
