#ifndef WPLAYWINDOW_H
#define WPLAYWINDOW_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QtAV>
#include <QtAVWidgets>
#include <QHBoxLayout>
#include <QCloseEvent>

class PlayCell :public QFrame
{
    Q_OBJECT
public:
    explicit PlayCell(QWidget *parent = 0);
    ~PlayCell();

protected:
    //virtual void paintEvent(QPaintEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    QLabel *lb;
    QVBoxLayout *mainLayout;

    QtAV::AVPlayer *Player;
    QtAV::GLWidgetRenderer2 *Renderer;
    QLabel *title;
};

class WPlayWindow : public QFrame
{

    Q_OBJECT

public:
    enum ViewType{
        ViewType_1,
        ViewType_4,
        ViewType_9,
        ViewType_16
    };

    explicit WPlayWindow(QWidget *parent = 0);
    void setViewType(ViewType type);

protected:

private:
    void readSetting();
    void writeSetting(ViewType type);

private:
    QList<QWidget *> cellGroup;
    QGridLayout *mainLayout;
};

Q_DECLARE_METATYPE(WPlayWindow::ViewType)

#endif // WPLAYWINDOW_H
