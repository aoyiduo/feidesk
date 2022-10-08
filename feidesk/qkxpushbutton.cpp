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

#include "qkxpushbutton.h"

#include <QMouseEvent>
#include <QMenu>

QKxPushButton::QKxPushButton(QWidget *parent)
    : QPushButton(parent)
{
    setText("     |");
}

void QKxPushButton::mousePressEvent(QMouseEvent *e)
{
    QRect rt = rect();
    QPoint pt = e->pos();
    QMenu *m = menu();
    m_hasClicked = false;
    if(m != nullptr) {
        int w = qMin(rt.width() / 2, rt.height());
        rt.setRight(rt.right() - w);
        if(rt.contains(pt)) {
            emit clicked();
            m_hasClicked = true;
            e->setAccepted(true);
            return;
        }
    }
    QPushButton::mousePressEvent(e);
}

void QKxPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_hasClicked) {
        e->setAccepted(true);
        return;
    }
    QPushButton::mousePressEvent(e);
}

void QKxPushButton::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
}
