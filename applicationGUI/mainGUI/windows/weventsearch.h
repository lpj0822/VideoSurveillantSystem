#ifndef WEVENTSEARCH_H
#define WEVENTSEARCH_H

#include <QFrame>
#include <QDateTimeEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "wtitletableview.h"


class WEventSearch : public QFrame
{
    Q_OBJECT
public:
    explicit WEventSearch(QWidget *parent = 0);

    enum EventInfoIndex{
        EventInfo_ID = 0,
        EventInfo_ResourceType,
        EventInfo_ResourcePath ,
        EventInfo_EventType,
        EventInfo_EventName,
        EventInfo_EventTime,
        EventInfo_Area,
        EventInfo_Camera,
    };

signals:

public slots:

private slots:
    void onBtnSearch();
    void on_EventImport();
    void on_EventExport();

private:
    void initData();
    void initUI();
    void initConnect();
    bool databaseConnect(const QString &dbName);



private:
    QDateTimeEdit *beginTime;
    QDateTimeEdit *endTime;

    QToolButton *tBtnSearch;
    QToolButton *tBtnImportEvent;
    QToolButton *tBtnExportEvent;

    WTitleTableView *tableView;
    QSqlTableModel *tableModel;
    QSqlDatabase previewInfoDatabase;
};

#endif // WEVENTSEARCH_H
