#ifndef WLOCALRESOURCE_H
#define WLOCALRESOURCE_H

#include <QFrame>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QMenu>
#include <QSqlTableModel>
#include <QSqlQuery>
#include "../windows/searchbtn.h"
#include "../windows/wtitletableview.h"

class WLocalResource : public QFrame
{
    Q_OBJECT
public:
    explicit WLocalResource(int function, QWidget *parent = 0);
    ~WLocalResource();

signals:

public slots:

private slots:
    void slotAnalysisResult();
    void on_Clicked(QModelIndex index);
    void on_treeViewAreaMenuTriggered(QAction *act);
    void on_tableViewTriggered(QAction *act);
    void on_treeViewMenuTriggered(QAction *act);
    void on_ShowMouseRightButton(const QPoint &pos);
    void on_TableViewRightButton(const QPoint &pos);

private:
    void initData();
    void initUI();
    void initConnect();
    void initDataBase();
    bool removeFolderContent(const QString &folderDir);
    bool copyRecursively(const QString &srcFilePath,const QString &tgtFilePath);
    bool databaseCreate();
    bool databaseConnect(const QString &dbName);
    void addOneRowToFileSYS(const QString &msg);
    QString sizeHuman(int size);


private:
    SearchBtn *searchBtn;

    QTreeView *treeView;
    QFileSystemModel *dirModel;
    QSqlTableModel *sqlTableModel;

    WTitleTableView *tableView;
    QSqlDatabase localFileSYSInfoDatabase;

    QMenu *treeViewMenu;
    QAction *actNewChild;
    QAction *actFixGroup;
    QAction *actDelGroup;
    QAction *actImportResource;

    QMenu *treeViewAreaMenu;
    QAction *actNewGroup;

    QMenu *tableViewMenu;
    QAction *actDelFile;
    QAction *actParse;

    int function;
};

#endif // WLOCALRESOURCE_H
