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

#ifndef QKXOPTIONFORM_H
#define QKXOPTIONFORM_H

#include <QWidget>

namespace Ui {
class QKxOptionForm;
}

class QKxOptionForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxOptionForm(QWidget *parent = 0);
    ~QKxOptionForm();

private:
    Ui::QKxOptionForm *ui;
};

#endif // QKXOPTIONFORM_H
