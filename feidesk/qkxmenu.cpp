/*******************************************************************************************
*
* Copyright (C) 2022 Guangzhou AoYiDuo Network Technology Co.,Ltd. All Rights Reserved.
*
* Contact: http://www.aoyiduo.com
*
*   this file is used under the terms of the GPLv3[GNU GENERAL PUBLIC LICENSE v3]
* more information follow the website: https://www.gnu.org/licenses/gpl-3.0.en.html
*
*******************************************************************************************/

#include "qkxmenu.h"

#include <QEvent>
#include <QToolTip>
#include <QHelpEvent>

QKxMenu::QKxMenu(QWidget *parent)
    : QMenu(parent)
{

}

QKxMenu::~QKxMenu()
{

}

bool QKxMenu::event(QEvent *e)
{
    if(e->type() == QEvent::ToolTip) {
        //show action tooltip instead of widget tooltip
        QHelpEvent* he = dynamic_cast<QHelpEvent*>(e);
        QAction* act = actionAt(he->pos());
        if( act ) {
            QToolTip::showText(he->globalPos(), act->toolTip(), this);
            return true;
        }
    }
    return QMenu::event(e);
}
