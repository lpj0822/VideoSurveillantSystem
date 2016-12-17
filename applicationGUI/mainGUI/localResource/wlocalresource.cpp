#pragma execution_character_set("utf-8")
#include "wlocalresource.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QFileInfo>
#include "../expand/myhelper.h"
#include "vehicleconverseparse.h"
#include "leaveparse.h"

WLocalResource::WLocalResource(int function, QWidget *parent) : QFrame(parent)
{
    initData();
    initUI();
    initConnect();
    this->function = function;
}

WLocalResource::~WLocalResource()
{
    localFileSYSInfoDatabase.close();
}

void WLocalResource::initData()
{
    QDir *temp = new QDir;
    bool exist = temp->exists(QDir::currentPath()+"/local");
    if(!exist)
        temp->mkdir(QDir::currentPath()+"/local");

    treeViewMenu = new QMenu();
    actNewChild = new QAction("新建子资源组",this);
    actFixGroup = new QAction("重命名",this);
    actDelGroup = new QAction("删除资源组",this);
    actImportResource = new QAction("导入资源",this);
    treeViewMenu->addAction(actNewChild);
    treeViewMenu->addAction(actFixGroup);
    treeViewMenu->addAction(actDelGroup);
    treeViewMenu->addSeparator();
    treeViewMenu->addAction(actImportResource);

    treeViewAreaMenu = new QMenu();
    actNewGroup = new QAction("新建资源组",this);
    treeViewAreaMenu->addAction(actNewGroup);

    tableViewMenu = new QMenu();
    actDelFile = new QAction("删除资源",this);
    actParse = new QAction("智能分析",this);
    tableViewMenu->addAction(actDelFile);
    tableViewMenu->addAction(actParse);

    searchBtn = new SearchBtn("搜索");

    dirModel = new QFileSystemModel;
    dirModel->setRootPath(QDir::currentPath());
    dirModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    treeView = new QTreeView(this);
    treeView->setModel(dirModel);
    treeView->setRootIndex(dirModel->index(QDir::currentPath()+"/local"));
    treeView->setColumnHidden(1,true);
    treeView->setColumnHidden(2,true);
    treeView->setColumnHidden(3,true);
    treeView->header()->setVisible(false);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    initDataBase();
    tableView = new WTitleTableView(sqlTableModel,"资源列表");
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->hideColumn(0);
}

void WLocalResource::initUI()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("资源组"));
    leftLayout->addWidget(treeView);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(tableView);

    mainLayout->setStretch(0,0);
    mainLayout->setStretch(1,1);
    this->setLayout(mainLayout);
}

void WLocalResource::initConnect()
{
    connect(treeView, &QTreeView::clicked, this,&WLocalResource::on_Clicked);
    connect(treeViewMenu, SIGNAL(triggered(QAction *)),this,SLOT(on_treeViewMenuTriggered(QAction *)));
    connect(treeViewAreaMenu, SIGNAL(triggered(QAction *)),this,SLOT(on_treeViewAreaMenuTriggered(QAction *)));
    connect(tableViewMenu, SIGNAL(triggered(QAction *)),this,SLOT(on_tableViewTriggered(QAction*)));
    connect(tableView, SIGNAL(customContextMenuRequested(const QPoint&)),this,
            SLOT(on_TableViewRightButton(const QPoint&)));

    connect(treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this,
            SLOT(on_ShowMouseRightButton(const QPoint &)));
}

void WLocalResource::on_Clicked(QModelIndex index)
{
    sqlTableModel->setFilter(QString("FilePath = '%1'").arg(dirModel->filePath(index)));
    sqlTableModel->select();
}

void WLocalResource::on_ShowMouseRightButton(const QPoint &pos)
{
    QModelIndex idx = treeView->indexAt(pos);
    QString tmpFilePath = dirModel->filePath(idx);
    if(tmpFilePath.count() == 0){
        treeViewAreaMenu->popup(treeView->mapToGlobal(pos));
    }
    else{
        treeViewMenu->popup(treeView->mapToGlobal(pos));
    }
}

void WLocalResource::on_TableViewRightButton(const QPoint &pos)
{
    QPoint tempPos = pos;
    tempPos.setY(tempPos.y()-22-tableView->tableView->horizontalHeader()->height());

    QModelIndex idx = tableView->tableView->indexAt(tempPos);
    if(idx.isValid()){
        tableViewMenu->popup(tableView->mapToGlobal(pos));
    }
}

void WLocalResource::on_treeViewAreaMenuTriggered(QAction *act)
{
    if(act==actNewGroup){
        QString name = QInputDialog::getText(this, tr("创建资源组"), tr("资源组名字"));
        if(name.isEmpty())
            return;
        QDir temp;
        bool exist = temp.exists(QDir::currentPath()+"/local"+"/"+name);
        if(exist)
            QMessageBox::warning(this,tr("创建资源组"),tr("资源组已经存在！"));
        else
        {
            bool ok = temp.mkdir(QDir::currentPath()+"/local"+"/"+name);
            if(!ok)
                QMessageBox::warning(this,tr("创建资源组"),tr("资源组创建失败！"));
        }
    }
}

void WLocalResource::on_treeViewMenuTriggered(QAction *act)
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    if(act==actNewChild){
        QString dirName = QInputDialog::getText(this,tr("创建目录"),tr("目录名字"));
        if (!dirName.isEmpty()) {
            if (!dirModel->mkdir(index, dirName).isValid()) {
                QMessageBox::warning(this,tr("创建目录"),tr("创建目录失败"));
            }
        }
    }
    else if(act==actFixGroup){
        QString fileName = QInputDialog::getText(this,tr("修改目录"),tr("目录名字"));
        QFile file(dirModel->filePath(index));
        QDir dir(dirModel->filePath(index));
        dir.cdUp();
        file.rename(dir.path()+"/"+fileName);
        qDebug()<<"rename ok";
    }
    else if(act==actDelGroup){
        int ret = QMessageBox::information(this, tr("Application"),
                                           tr("您确定删除此目录及其所包含的内容"),
                                           QMessageBox::Ok,
                                           QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Ok){
            if(removeFolderContent(dirModel->filePath(index))){
                qDebug()<<"Remove successfully";
            }
        }
        else if (ret == QMessageBox::Cancel){
            return;
        }
    }
    else if(act==actImportResource){
        QStringList  fileNames = QFileDialog::getOpenFileNames(this,"选择导入的资源文件",".","*.mp4 *.rmvb *.avi");
        qDebug()<<dirModel->filePath(index);
        for(QString src:fileNames){
            QFileInfo fi(src);
            QString targetPath = dirModel->filePath(index) +"/"+ fi.fileName();
            if(copyRecursively(src,targetPath)){
                qDebug()<<"copy okkkkkkkkk";
            }
        }
    }
}

void WLocalResource::on_tableViewTriggered(QAction *act)
{
    QModelIndex index = tableView->tableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString path = sqlTableModel->index(index.row(),5).data().toString();
    QString baseName = sqlTableModel->index(index.row(),1).data().toString();
    QString suffixName = sqlTableModel->index(index.row(),2).data().toString();

    if(act==actDelFile){
        QFile file(path+"/"+baseName+"."+suffixName);
        if(file.remove()){
            sqlTableModel->removeRow(index.row());
            sqlTableModel->select();
        }
    }
    else if (act==actParse){
        switch (this->function){

        case 1:
        {
            LeaveParse *parseDialog = new LeaveParse(this);
            parseDialog->playVideo(path+"/"+baseName+"."+suffixName);
            connect(parseDialog, &LeaveParse::signal_NewPreviewInfo, this, &WLocalResource::slotAnalysisResult);
            parseDialog->exec();
            parseDialog->deleteLater();
        }
            break;
        case 2:
            break;
        case 3:
        {
            VehicleConverseParse *parseDialog = new VehicleConverseParse(this);
            parseDialog->playVideo(path+"/"+baseName+"."+suffixName);
            connect(parseDialog, &VehicleConverseParse::signal_NewPreviewInfo, this, &WLocalResource::slotAnalysisResult);
            parseDialog->exec();
            parseDialog->deleteLater();
        }
            break;
        case 4:
            break;
        }
    }
}

void WLocalResource::slotAnalysisResult()
{
    qDebug() << "Analysis result";
}

bool WLocalResource::copyRecursively(const QString &srcFilePath,const QString &tgtFilePath)
{
    char* byteTemp = new char[4096];
    int fileSize = 0;
    int totalCopySize = 0;
    QFile tofile;
    tofile.setFileName(tgtFilePath);
    if(!tofile.open(QIODevice::WriteOnly)){
        qDebug() << "open tofile failed！！！";
        return false;
    }

    QDataStream out(&tofile);
    out.setVersion(QDataStream::Qt_5_5);

    QFile fromfile;
    fromfile.setFileName(srcFilePath);
    if(!fromfile.open(QIODevice::ReadOnly)){
        qDebug() << "open fromfile failed！！！";
        return false;
    }
    fileSize = fromfile.size();
    QDataStream in(&fromfile);

    in.setVersion(QDataStream::Qt_5_5);

    QProgressDialog *progressDlg=new QProgressDialog(this);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(5);
    progressDlg->setWindowTitle(tr("请等待"));
    progressDlg->setLabelText(tr("正在复制......      "));
    progressDlg->setCancelButtonText(tr("取消"));
    progressDlg->setRange(0,fileSize);

    while(!in.atEnd())
    {
        int readSize = 0;
        readSize = in.readRawData(byteTemp, 4096);
        out.writeRawData(byteTemp, readSize);
        totalCopySize += readSize;
        progressDlg->setValue(totalCopySize);
        if(progressDlg->wasCanceled())
            return false;
    }
    if(totalCopySize == fileSize){
        fromfile.close();
        tofile.close();
        QFileInfo targetFile(tgtFilePath);

        sqlTableModel->setFilter(QString("FilePath = '%1' and FileName = '%2' and FileType = '%3'")
                                 .arg(targetFile.path()).arg(targetFile.baseName()).arg(targetFile.suffix()));
        if(sqlTableModel->rowCount() == 0){
            sqlTableModel->insertRow(0);
            sqlTableModel->setData(sqlTableModel->index(0, 1), targetFile.baseName());
            sqlTableModel->setData(sqlTableModel->index(0, 2), targetFile.suffix());
            sqlTableModel->setData(sqlTableModel->index(0, 3), sizeHuman(targetFile.size()));
            sqlTableModel->setData(sqlTableModel->index(0, 4), targetFile.created().toString("yyyy-MM-dd hh:mm:ss"));
            sqlTableModel->setData(sqlTableModel->index(0, 5), targetFile.path());
            sqlTableModel->submitAll();
        }
        else {
            qDebug()<<"有重复的，不需要添加了";
        }
        sqlTableModel->setFilter(QString("FilePath = '%1'").arg(targetFile.path()));
        return true;
    }
    else
        return false;
}

bool WLocalResource::removeFolderContent(const QString &folderDir)
{
    QDir directory(folderDir);
    if (!directory.exists()){
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(folderDir);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
            else {//succefully
                sqlTableModel->setFilter(QString("FilePath = '%1' and FileName = '%2' and FileType = '%3'")
                                         .arg(fileInfo.path()).arg(fileInfo.baseName()).arg(fileInfo.suffix()));

                if(sqlTableModel->rowCount()==1){
                    sqlTableModel->removeRow(0);
                }
                //sqlTableModel->select();
                qDebug() << "remove file" << filePath << " okkkkkk!";
            }
        }
        else if (fileInfo.isDir())
        {
            if (!removeFolderContent(filePath))
            {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;
}

void WLocalResource::initDataBase()
{
    if(databaseConnect(QString(QDir::currentPath()+"/LocalFileSYS"))){
        QSqlQuery query(localFileSYSInfoDatabase);
        query.exec(MyHelper::strCreateLocalFileInfo);

        sqlTableModel = new QSqlTableModel(this,localFileSYSInfoDatabase);
        sqlTableModel->setTable("LocalFileInfo");
        sqlTableModel->setSort(0, Qt::AscendingOrder);
        sqlTableModel->setHeaderData(1, Qt::Horizontal, "文件名称");
        sqlTableModel->setHeaderData(2, Qt::Horizontal, "文件类型");
        sqlTableModel->setHeaderData(3, Qt::Horizontal, "文件大小");
        sqlTableModel->setHeaderData(4, Qt::Horizontal, "创建时间");
        sqlTableModel->setHeaderData(5, Qt::Horizontal, "路径");
        sqlTableModel->setFilter("IKUSDHYFUIKUHFK");
    }
}

bool WLocalResource::databaseConnect(const QString &dbName)
{
    localFileSYSInfoDatabase = QSqlDatabase::addDatabase("QSQLITE","LocalFileSearch");
    localFileSYSInfoDatabase.setDatabaseName(dbName);
    if (!localFileSYSInfoDatabase.open()) {
        return false;
    }
    return true;
}

QString WLocalResource::sizeHuman(int size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";
    QStringListIterator i(list);
    QString unit("bytes");
    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}
