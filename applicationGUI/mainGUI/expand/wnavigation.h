#ifndef WNAVIGATION_H
#define WNAVIGATION_H

#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QFrame>

class WNavigation : public QFrame
{
    Q_OBJECT
public:
    explicit WNavigation(QWidget *parent = 0);
    ~WNavigation();
    void AddBtn(const QString &str, const QIcon &icon);

signals:
    void signalBtnClicked(int index);

private slots:
    void slotBtnClicked();

private:
    QList<QToolButton *> btnGroup;
    QVBoxLayout *mainLayout;
    int btnWidth;
};

#endif // WNAVIGATION_H
