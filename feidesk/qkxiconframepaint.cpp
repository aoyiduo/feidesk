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

#include "qkxiconframepaint.h"

#include <QApplication>
#include <QPainter>

QKxIconFramePaint::QKxIconFramePaint(QWidget *parent)
    : QStyledItemDelegate(parent)
{

}

void QKxIconFramePaint::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &idx) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, idx);
    QRect rt = opt.rect;
    rt = rt.adjusted(2, 2, -2, -2);
    opt.rect = rt.adjusted(2, 2, -2, -2);
    const QWidget *widget = NULL;
    widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    QFlags<QStyle::StateFlag> state = QStyle::State_Selected;
    if (opt.state & state) {
        //painter->setPen(QColor(18,150,219));
        painter->setPen(QColor(Qt::white));
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(rt, 5, 5, Qt::AbsoluteSize);
    }
}
