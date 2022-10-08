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

#ifndef QKXICONFRAMEPAINT_H
#define QKXICONFRAMEPAINT_H

#include <QStyledItemDelegate>

class QKxIconFramePaint : public QStyledItemDelegate
{
public:
    explicit QKxIconFramePaint(QWidget *parent = 0);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &idx) const;
};

#endif // QKXICONFRAMEPAINT_H
