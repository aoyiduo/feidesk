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

#include "qkxoptionform.h"
#include "ui_qkxoptionform.h"

QKxOptionForm::QKxOptionForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QKxOptionForm)
{
    ui->setupUi(this);
}

QKxOptionForm::~QKxOptionForm()
{
    delete ui;
}
