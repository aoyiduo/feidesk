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

#ifndef QKXPUSHBUTTON_H
#define QKXPUSHBUTTON_H


#include <QPushButton>

class QKxPushButton : public QPushButton
{
public:
    explicit QKxPushButton(QWidget *parent = nullptr);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);
private:
    bool m_hasClicked;
};

#endif // QKXPUSHBUTTON_H
