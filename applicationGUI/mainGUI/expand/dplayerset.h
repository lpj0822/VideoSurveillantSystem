#ifndef DPLAYERSET_H
#define DPLAYERSET_H

#include <QDialog>
#include <QSlider>
#include <QPushButton>

class DPlayerSet : public QDialog
{
    Q_OBJECT
public:
    explicit DPlayerSet(QWidget *parent = 0);

signals:
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanegd(int);
    void saturationChanged(int);

public slots:

private:
    QSlider *mpBSlider;
    QSlider *mpCSlider;
    QSlider *mpSSlider;
    QSlider *mpHSlider;

    QPushButton *btnReset;
};

#endif // DPLAYERSET_H
