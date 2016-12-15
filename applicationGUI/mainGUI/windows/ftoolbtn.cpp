#pragma execution_character_set("utf-8")
#include "ftoolbtn.h"

FToolbtn::FToolbtn(const QString &text, const QIcon &icon, QWidget *parent):
    QToolButton(parent)
{
    setText(text);
    setIcon(icon);
    setIconSize(QSize(48, 48));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setCheckable(true);
    setFocusPolicy(Qt::NoFocus);
}

WToolbtn::WToolbtn(const QIcon &icon, QWidget *parent):
    QToolButton(parent)
{
    setIcon(icon);
    setIconSize(QSize(14, 14));
    setFocusPolicy(Qt::NoFocus);
}

