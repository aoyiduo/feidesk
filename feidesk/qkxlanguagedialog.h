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

#ifndef QKXLANGUAGEDIALOG_H
#define QKXLANGUAGEDIALOG_H

#include <QDialog>

namespace Ui {
class QKxLanguageDialog;
}

class QKxLanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QKxLanguageDialog(QWidget *parent = 0);
    ~QKxLanguageDialog();
private slots:
    void onButtonSaveClicked();
private:
    Ui::QKxLanguageDialog *ui;
};

#endif // QKXLANGUAGEDIALOG_H
