#ifndef INTRUDEDETECTCONTROLWINDOW_H
#define INTRUDEDETECTCONTROLWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QTextBrowser>
#include <QVector>
#include <QPoint>
#include "videoAnalysis/intrudeshowvideowindow.h"

class IntrudeDetectControlWindow : public QWidget
{
    Q_OBJECT
public:
    explicit IntrudeDetectControlWindow(QWidget *parent = 0);
    ~IntrudeDetectControlWindow();

    void drawingArea(QPixmap &pixmap);//绘制区域

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void slotOpenVideo();
    void slotStartDetection();
    void slotConfiguration();
    void slotClearText();

    void slotVideoMessage(bool isVideoOpen);
    void slotIntrudeMessage(int number, QString savePath);

private:

    QPushButton *openVideoButton;
    QPushButton *startDetectionButton;
    QPushButton *configurationButton;
    QPushButton *clearTextButton;

    QTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    IntrudeShowVideoWindow *lbPlay;//入侵检测

    QString path;//视频路径

    bool isOpen;//是否打开视频
    bool isStartDetection;//是否开始检测

    void initUI();
    void initData();
    void initConnect();
};

#endif // INTRUDEDETECTCONTROLWINDOW_H
