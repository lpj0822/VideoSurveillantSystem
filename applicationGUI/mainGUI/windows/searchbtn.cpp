#include "searchbtn.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include "../expand/myhelper.h"

#pragma execution_character_set("utf-8")

SearchBtn::SearchBtn(const QString &linetext, QWidget *parent)
    : QLineEdit(parent)
{
    QPushButton *btn = new QPushButton();
    btn->setIcon(MyHelper::getAwesome()->icon(fa::search));

    btn->setFocusPolicy(Qt::NoFocus);
    btn->setFlat(true);
    btn->setCursor(QCursor(Qt::PointingHandCursor));
    btn->setStyleSheet("background-color: rgb(0, 170, 255);border: 0px");
    setPlaceholderText(linetext);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 2, 2, 2);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btn);
    setLayout(buttonLayout);
    connect(this,&SearchBtn::returnPressed,
            [&](){
        emit signal_beginToSreach(this->text());
        qDebug()<<this->text();
    });

    connect(btn,&QPushButton::clicked,
            [&](){
        emit signal_beginToSreach(this->text());
        qDebug()<<this->text();
    });
}
