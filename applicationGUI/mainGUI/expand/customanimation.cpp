#include "customanimation.h"

CustomAnimation::CustomAnimation(QObject *target1, QObject *target2, const QByteArray &propertyName, QObject *parent) : QObject(parent)
{
    animation1 = new QPropertyAnimation(target1,propertyName,this);
    animation1->setDuration(400);
    //animation1->setPropertyName(propertyName);
    animation2 = new QPropertyAnimation(target2,propertyName,this);
    animation2->setDuration(400);
    //animation2->setPropertyName(propertyName);

    group = new QParallelAnimationGroup(this);
}

void CustomAnimation::setTargetObject(QObject *target1,QObject *target2)
{
    animation1->setTargetObject(target1);
    animation1->setTargetObject(target2);
}

void CustomAnimation::setStartValue(const QVariant &value1,const QVariant &value2)
{
    animation1->setStartValue(value1);
    animation2->setStartValue(value2);
}

void CustomAnimation::setEndValue(const QVariant &value1,const QVariant &value2)
{
    animation1->setEndValue(value1);
    animation2->setEndValue(value2);
}

void CustomAnimation::startAnimation()
{
    group->addAnimation(animation1);
    group->addAnimation(animation2);
    group->start();
}
