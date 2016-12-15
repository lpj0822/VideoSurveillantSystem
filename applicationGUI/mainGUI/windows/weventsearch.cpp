#include "weventsearch.h"
#include "../expand/myhelper.h"
#include "wshowinfodetail.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QSqlError>
#include <QFileInfo>

#pragma execution_character_set("utf-8")

WEventSearch::WEventSearch(QWidget *parent) : QFrame(parent)
{
    initData();
    initUI();
    initConnect();
    tBtnSearch->click();
}

void WEventSearch::initData()
{
    QtAwesome* awesome = MyHelper::getAwesome();//a single awesome
    beginTime = new QDateTimeEdit(this);
    endTime = new QDateTimeEdit(this);

    beginTime->setDate(QDate::currentDate().addDays(-1));
    beginTime->setTime(QTime::currentTime());
    beginTime->setCalendarPopup(true);
    beginTime->setDisplayFormat(QApplication::translate("WEventSearch", "yyyy-MM-dd HH:mm:ss", 0));

    endTime->setDate(QDate::currentDate());
    endTime->setTime(QTime::currentTime());
    endTime->setCalendarPopup(true);
    endTime->setDisplayFormat(QApplication::translate("WEventSearch", "yyyy-MM-dd HH:mm:ss", 0));

    tBtnSearch = new QToolButton(this);
    tBtnSearch->setIcon(awesome->icon(fa::search));
    tBtnSearch->setText("查询");
    tBtnSearch->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    tBtnImportEvent = new QToolButton(this);
    tBtnImportEvent->setIcon(awesome->icon(fa::clouddownload));
    tBtnImportEvent->setText("导入信息");
    tBtnImportEvent->setIconSize(QSize(20,20));
    tBtnImportEvent->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    tBtnExportEvent = new QToolButton(this);
    tBtnExportEvent->setIcon(awesome->icon(fa::cloudupload));
    tBtnExportEvent->setText("备份信息");
    tBtnExportEvent->setIconSize(QSize(20,20));
    tBtnExportEvent->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    if(databaseConnect(QString(QDir::currentPath()+"/"+"Event"))){
        QSqlQuery query(previewInfoDatabase);
        query.exec(MyHelper::strCreateEventInfo);

        tableModel = new QSqlTableModel(this,previewInfoDatabase);
        tableModel->setTable("eventInfo");
        tableModel->setSort(0, Qt::AscendingOrder);

        tableModel->setHeaderData(EventInfo_EventTime, Qt::Horizontal, "信息时间");
        tableModel->setHeaderData(EventInfo_Area, Qt::Horizontal, "区域");
        tableModel->setHeaderData(EventInfo_Camera, Qt::Horizontal, "监控点");
        tableModel->select();

        tableView = new WTitleTableView(tableModel,"信息列表");
        tableView->hideColumn(EventInfo_ID);
        tableView->hideColumn(EventInfo_ResourceType);
        tableView->hideColumn(EventInfo_ResourcePath);
        tableView->hideColumn(EventInfo_EventType);
        tableView->hideColumn(EventInfo_EventName);

    }
    //this->setFrameShape(QFrame::HLine);
//    this->setFrameShape(QFrame::Box);
//    this->setLineWidth(1);
}

void WEventSearch::initUI()
{
    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addStretch(0);
    timeLayout->setMargin(0);
    QLabel *timelbBegin = new QLabel("开始时间：");
    QLabel *timelbEnd = new QLabel("结束时间：");
    timeLayout->addWidget(timelbBegin);
    timeLayout->addWidget(beginTime);
    timeLayout->addWidget(timelbEnd);
    timeLayout->addWidget(endTime);
    timeLayout->addWidget(tBtnSearch);

    QVBoxLayout *layoutleft = new QVBoxLayout();
    layoutleft->setMargin(0);
    layoutleft->setSpacing(0);
    layoutleft->addLayout(timeLayout);

    QVBoxLayout *layoutright = new QVBoxLayout();
    layoutright->setMargin(0);
    layoutright->setSpacing(0);
    layoutright->addWidget(tBtnImportEvent);
    layoutright->addWidget(tBtnExportEvent);

    QHBoxLayout *aboveLayout = new QHBoxLayout();
    aboveLayout->setMargin(0);
    aboveLayout->addStretch(0);
    aboveLayout->addLayout(layoutleft);
    aboveLayout->addStretch(0);
    aboveLayout->addLayout(layoutright);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(6,2,6,0);
    //mainLayout->setSpacing(1);
    mainLayout->addLayout(aboveLayout);
    mainLayout->addWidget(tableView);

    this->setLayout(mainLayout);
}

void WEventSearch::initConnect()
{
    connect(tableView->tableView,&QTableView::doubleClicked,
            [&](const QModelIndex &index){

                int row = index.row();
                qDebug() <<tableModel->index(row,1).data().toString();
                WShowInfoDetail *showInfoDetail = new WShowInfoDetail(this);
                showInfoDetail->setEventTime(tableModel->index(row,EventInfo_EventTime).data().toString());
                showInfoDetail->setArea(tableModel->index(row,EventInfo_Area).data().toString());
                showInfoDetail->setCamera(tableModel->index(row,EventInfo_Camera).data().toString());
                //showInfoDetail->playVideo(tableModel->index(row,EventInfo_ResourcePath).data().toString());
                showInfoDetail->play(tableModel->index(row,WEventSearch::EventInfo_ResourcePath).data().toString(),
                                     tableModel->index(row,WEventSearch::EventInfo_ResourceType).data().toString());
                showInfoDetail->setEnablJudgeSured(false);
                showInfoDetail->setTitle("信息详情");
                showInfoDetail->show();
    });

    connect(tBtnSearch,&QToolButton::clicked,this,&WEventSearch::onBtnSearch);
    connect(tBtnImportEvent,&QToolButton::clicked,this,&WEventSearch::on_EventImport);
    connect(tBtnExportEvent,&QToolButton::clicked,this,&WEventSearch::on_EventExport);
}

bool WEventSearch::databaseConnect(const QString &dbName)
{
    previewInfoDatabase = QSqlDatabase::addDatabase("QSQLITE","eventSearch");
    previewInfoDatabase.setDatabaseName(dbName);
    if (!previewInfoDatabase.open()) {
        return false;
    }
    return true;
}

void WEventSearch::onBtnSearch()
{
    QString time1 = beginTime->text();
    QString time2 = endTime->text();

    qDebug()<<QString("eventTime between '%1' and '%2'").arg(time1).arg(time2);
    tableModel->setFilter(QString("eventTime between '%1' and '%2'").arg(time1).arg(time2));

    if(tableModel->select())
        qDebug()<<"ok......";
    else{
        qDebug()<<tableModel->lastError();
    }
}

void WEventSearch::on_EventImport()
{
    QString name = QFileDialog::getOpenFileName(this,"导入信息","选择导入的信息文件");
    if(name.isEmpty())
        return;
    QSqlDatabase Database = QSqlDatabase::addDatabase("QSQLITE","eventImport");


    Database.setDatabaseName(name);
    if (!Database.open()) {
        return;
    }

    QSqlTableModel Model(this,Database);
    Model.setTable("eventInfo");
    Model.select();//this is very important

    auto rowSize = Model.rowCount();

    for(auto i=0;i<rowSize;i++){
        tableModel->insertRow(0);
        tableModel->setData(tableModel->index(0, EventInfo_ResourceType), Model.index(i,EventInfo_ResourceType).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_ResourcePath), Model.index(i,EventInfo_ResourcePath).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_EventType), Model.index(i,EventInfo_EventType).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_EventName), Model.index(i,EventInfo_EventName).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_EventTime), Model.index(i,EventInfo_EventTime).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_Area), Model.index(i,EventInfo_Area).data().toString());
        tableModel->setData(tableModel->index(0, EventInfo_Camera), Model.index(i,EventInfo_Camera).data().toString());
        tableModel->submitAll();
    }
    tBtnSearch->click();
}

void WEventSearch::on_EventExport()
{
    QString name = QFileDialog::getSaveFileName(this,"导出信息","ExportEvent");
    QFile::copy(QString(QDir::currentPath()+"/"+"Event"),name);
}
