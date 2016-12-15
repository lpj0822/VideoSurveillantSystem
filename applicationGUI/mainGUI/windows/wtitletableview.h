#ifndef WTITLETABLEVIEW_H
#define WTITLETABLEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QTableView>
#include <QHBoxLayout>

class WTitleTableView : public QWidget
{
    Q_OBJECT
public:
    explicit WTitleTableView(QAbstractItemModel *model,const QString &str = 0,QWidget *parent = 0);
    void setTitle(const QString &str);
    void addWidget(QWidget *w);
    void hideColumn(int index);

    QTableView *tableView;

signals:

public slots:

private:
    void initData();
    void initUI();
    void initConnect();

private:
    QLabel *title;
    QHBoxLayout *titleLayout;
};

#endif // WTITLETABLEVIEW_H
