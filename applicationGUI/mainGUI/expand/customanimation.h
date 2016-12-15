#ifndef CUSTOMANIMATION_H
#define CUSTOMANIMATION_H

#include <QObject>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class CustomAnimation : public QObject
{
    Q_OBJECT
public:
    explicit CustomAnimation(QObject *target1,QObject *target2,const QByteArray &propertyName = "geometry", QObject *parent = 0);
    void setTargetObject(QObject *target1,QObject *target2);
    void setStartValue(const QVariant &value1,const QVariant &value2);
    void setEndValue(const QVariant &value1,const QVariant &value2);
    void startAnimation();

signals:

public slots:

private:
    QPropertyAnimation *animation1;
    QPropertyAnimation *animation2;

    QParallelAnimationGroup *group;
};

#endif // CUSTOMANIMATION_H
