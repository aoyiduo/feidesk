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

#ifndef QKXSIMPLEMAINWINDOW_H
#define QKXSIMPLEMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class QKxSimpleMainWindow;
}

class QKxSimpleMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QKxSimpleMainWindow(QWidget *parent = 0);
    ~QKxSimpleMainWindow();

private:
    Ui::QKxSimpleMainWindow *ui;
};

#endif // QKXSIMPLEMAINWINDOW_H
