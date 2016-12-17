#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QDebug>

class VideoLabel : public QWidget
{
    Q_OBJECT
public:
    explicit VideoLabel(QWidget *parent = 0): QWidget(parent){
        label = new QLabel(this);
        label->setScaledContents(true);
    }

    QLabel* label;

signals:

protected:
    virtual void resizeEvent(QResizeEvent *e){
        label->resize(this->size());
        QWidget::resizeEvent(e);
    }
    virtual void mousePressEvent(QMouseEvent *e){
        if(e->button() == Qt::LeftButton){
            qDebug()<<"check on";
        }
        QWidget::mousePressEvent(e);
    }

private:

};

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);

signals:

private slots:
    void ChangeView1();
    void ChangeView4();
    void ChangeView9();
    void ChangeView16();

protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);

private:
    QList<VideoLabel *> labellist;
    QList<QHBoxLayout *> layoutlist;
    QGridLayout *gridlayout;
    int VideoType;
    void Initlabel();
    void InitRightPressMenu();
    void RemoveAllLayout();

    //menu
    QMenu *RightPressMenu;

};

#endif // MAINWIDGET_H
