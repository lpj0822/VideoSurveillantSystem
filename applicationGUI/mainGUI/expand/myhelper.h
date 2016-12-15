#ifndef MYHELPER_H
#define MYHELPER_H

#include <QtCore>
#include <QDesktopWidget>
#include <QApplication>
#include "../QtAwesome/QtAwesome.h"

class MyHelper: public QObject
{
public:
    explicit MyHelper();
    static QtAwesome* getAwesome();
    static const int title_height = 90;
    static QSize maxWindowSize;
    static bool isMaxSize;
    static QString strAPPName;
    static QString strAreaTree;
    static QString strViewType;
    static QString strCreateCameraEventInfo;
    static QString strCreatePreviewInfo;
    static QString strCreateLocalFileInfo;
    static QString strCreateEventInfo;
    static QString strCreateServerInfo;

private:
    static QtAwesome* m_awesome;
};

#endif // MYHELPER_H
