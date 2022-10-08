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

#ifndef QKXICONLABELBUTTON_H
#define QKXICONLABELBUTTON_H

#include <QAbstractButton>
#include <QIcon>

class QStyleOptionButton;
class QKxIconLabelButton : public QAbstractButton
{
public:
    explicit QKxIconLabelButton(const QIcon& icon, const QString& txt, QWidget *parent = nullptr);
    ~QKxIconLabelButton();
    void setTextColor(const QColor& clrNormal, const QColor& clrActive);
    void setBackgroundColor(const QColor& clrActive);
    void setItemSpacing(int space);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QColor m_clrTxtNormal, m_clrTxtActive;
    QColor m_clrBgActive;
    int m_spacing;
};

#endif // QKXICONLABELBUTTON_H
