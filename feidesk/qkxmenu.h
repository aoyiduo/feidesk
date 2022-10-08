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

#ifndef QKXMENU_H
#define QKXMENU_H

#include <QMenu>

class QKxMenu : public QMenu
{
public:
    explicit QKxMenu(QWidget *parent = nullptr);
    virtual ~QKxMenu();
protected:
    virtual bool event(QEvent *e);
};

#endif // QKXMENU_H
