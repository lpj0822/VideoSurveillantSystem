#ifndef WSERVER_H
#define WSERVER_H

#include <QWidget>
#include <QToolButton>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "wtitletableview.h"

class WServer : public QFrame
{
    Q_OBJECT
public:
    explicit WServer(QWidget *parent = 0);

    enum ServerInfoIndex{
        ServerInfo_ID = 0,
        ServerInfo_IP,
        ServerInfo_port,
        ServerInfo_DeviceSN,
        ServerInfo_Name,
        ServerInfo_UserName,
        ServerInfo_Password
    };

signals:

public slots:

private:
    void initData();
    void initUI();
    void initConnect();
    bool databaseCreate();
    bool databaseConnect(const QString &dbName);

private:

    WTitleTableView *tableView;
    QSqlTableModel *tableModel;
    QSqlDatabase previewInfoDatabase;
};

#endif // WSERVER_H
