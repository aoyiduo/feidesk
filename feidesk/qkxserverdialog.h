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

#ifndef QKXSERVERDIALOG_H
#define QKXSERVERDIALOG_H

#include <QDialog>

namespace Ui {
class QKxServerDialog;
}

class QKxServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QKxServerDialog(QWidget *parent = 0);
    ~QKxServerDialog();

private slots:
    void onRestoreButtonClicked();
    void onSaveButtonClicked();
private:
    Ui::QKxServerDialog *ui;
};

#endif // QKXSERVERDIALOG_H
