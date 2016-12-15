#include "wserver.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include "../expand/myhelper.h"

#pragma execution_character_set("utf-8")

WServer::WServer(QWidget *parent) : QFrame(parent)
{
    initData();
    initUI();
    initConnect();
}

void WServer::initData()
{
    if(databaseConnect(QString(QDir::currentPath()+"/"+"Server"))){
        QSqlQuery query(previewInfoDatabase);
        query.exec(MyHelper::strCreateServerInfo);

        tableModel = new QSqlTableModel(this,previewInfoDatabase);
        tableModel->setTable("ServerInfo");
        tableModel->select();
        if(tableModel->rowCount()==0){
            for(auto i=0;i<10;i++){
                tableModel->insertRow(0);
                tableModel->setData(tableModel->index(0,ServerInfo_IP),"127.0.0.1");
                tableModel->setData(tableModel->index(0,ServerInfo_port),"6666");
                tableModel->setData(tableModel->index(0,ServerInfo_DeviceSN),"SUJ678YJG");
                tableModel->setData(tableModel->index(0,ServerInfo_Name),"服务器"+QString::number(i));
                tableModel->setData(tableModel->index(0,ServerInfo_UserName),"User");
                tableModel->setData(tableModel->index(0,ServerInfo_Password),"Password");
                tableModel->submitAll();
            }
        }
        tableModel->setSort(0, Qt::AscendingOrder);
        tableModel->setHeaderData(1, Qt::Horizontal, "ip");
        tableModel->setHeaderData(2, Qt::Horizontal, "端口");
        tableModel->setHeaderData(3, Qt::Horizontal, "设备序列号");
        tableModel->setHeaderData(4, Qt::Horizontal, "服务器名");
        tableModel->select();

        tableView = new WTitleTableView(tableModel,"服务器信息列表");
        tableView->hideColumn(0);
        tableView->hideColumn(5);
        tableView->hideColumn(6);
    }
    this->setFrameShape(QFrame::Box);
    this->setLineWidth(1);
}

void WServer::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(tableView);

    this->setLayout(mainLayout);
}

void WServer::initConnect()
{
    ;
}

bool WServer::databaseConnect(const QString &dbName)
{
    previewInfoDatabase = QSqlDatabase::addDatabase("QSQLITE","Server");
    previewInfoDatabase.setDatabaseName(dbName);
    if (!previewInfoDatabase.open()) {
        return false;
    }
    return true;
}
