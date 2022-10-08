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

#include "qkxiconlabelbutton.h"

#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionButton>

QKxIconLabelButton::QKxIconLabelButton(const QIcon& icon, const QString &txt, QWidget *parent)
    : QAbstractButton(parent)
{
    setText(txt);
    setIcon(icon);
    setCheckable(true);
    setAutoExclusive(true);
    setMouseTracking(true);
    QPalette pal = palette();
    m_clrTxtNormal = pal.color(QPalette::WindowText);
    m_clrTxtActive = m_clrTxtNormal;
    m_clrBgActive = QColor(Qt::lightGray);
    m_spacing = 10;
}

QKxIconLabelButton::~QKxIconLabelButton()
{

}

void QKxIconLabelButton::setTextColor(const QColor &clrNormal, const QColor &clrActive)
{
    m_clrTxtNormal = clrNormal;
    m_clrTxtActive = clrActive;
}

void QKxIconLabelButton::setBackgroundColor(const QColor &clrActive)
{
    m_clrBgActive = clrActive;
}

void QKxIconLabelButton::setItemSpacing(int space)
{
    m_spacing = space;
}

void QKxIconLabelButton::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QRect rt = rect();
    if(isChecked()) {
        p.fillRect(rt, m_clrBgActive);
    }
    QRect crt = contentsRect();
    QIcon ico = icon();
    QSize icoSize = iconSize();    
    QRect icoRt(crt.topLeft(), icoSize);
    icoRt.translate(0, (crt.height() - icoSize.height()) / 2);
    ico.paint(&p, icoRt);
    QRect txtRt(icoRt.right() + m_spacing, icoRt.top(), crt.width() - icoRt.width(), icoRt.height());
    p.drawText(txtRt, Qt::AlignVCenter|Qt::AlignLeft, text());
}
