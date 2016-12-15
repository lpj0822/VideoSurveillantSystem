#include "myhelper.h"
#include <QDir>

#pragma execution_character_set("utf-8")


QtAwesome* MyHelper::m_awesome = nullptr;
QSize MyHelper::maxWindowSize;
bool MyHelper::isMaxSize = false;
QString MyHelper::strAPPName = "MySoftDemo3";
QString MyHelper::strAreaTree = QDir::currentPath() + "/config/AreaTree.ini";
QString MyHelper::strViewType = QDir::currentPath() + "/config/ViewType.ini";
QString MyHelper::strCreateCameraEventInfo = "CREATE TABLE IF NOT EXISTS CameraEventInfo ("
                                             "id integer primary key autoincrement,"
                                             "Server nvarchar not null,"
                                             "Channel nvarchar not null,"
                                             "BeginTime time not null,"
                                             "EndTime time not null,"
                                             "ExportVideoOrImage nvarchar not null,"
                                             "ExportWeb nvarchar not null)";

QString MyHelper::strCreatePreviewInfo = "CREATE TABLE IF NOT EXISTS PreviewInfo ("
                                         "id integer primary key autoincrement,"
                                         "resourccType nvarchar not null,"
                                         "resourcePath nvarchar not null,"
                                         "eventType nvarchar not null,"
                                         "eventName nvarchar not null,"
                                         "eventTime datetime not null,"
                                         "Area nvarchar not null,"
                                         "Camera nvarchar not null)";

QString MyHelper::strCreateLocalFileInfo = "CREATE TABLE IF NOT EXISTS LocalFileInfo ("
                                           "id integer primary key autoincrement,"
                                           "FileName nvarchar not null,"
                                           "FileType nvarchar not null,"
                                           "FileSize nvarchar not null,"
                                           "CreatTime datetime not null,"
                                           "FilePath nvarchar not null)";

QString MyHelper::strCreateEventInfo = "CREATE TABLE IF NOT EXISTS EventInfo ("
                                            "id integer primary key autoincrement,"
                                            "resourceType nvarchar not null,"
                                            "resourcePath nvarchar not null,"
                                            "eventType nvarchar not null,"
                                            "eventName nvarchar not null,"
                                            "eventTime datetime not null,"
                                            "Area nvarchar not null,"
                                            "Camera nvarchar not null)";

QString MyHelper::strCreateServerInfo = "CREATE TABLE IF NOT EXISTS ServerInfo ("
                                        "id integer primary key autoincrement,"
                                        "ip nvarchar not null,"
                                        "port nvarchar not null,"
                                        "DeviceSN nvarchar not null,"
                                        "Name nvarchar not null,"
                                        "UserName nvarchar not null,"
                                        "Password nvarchar not null)";

MyHelper::MyHelper()
{
    ;
}

QtAwesome* MyHelper::getAwesome()
{
    if(!m_awesome)
    {
        m_awesome = new QtAwesome();
        m_awesome->initFontAwesome();
        m_awesome->setDefaultOption( "color", "#FFFFFF" );
    }
    return m_awesome;
}
