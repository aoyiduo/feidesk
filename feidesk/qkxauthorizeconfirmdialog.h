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

#ifndef QKXAUTHORIZECONFIRMDIALOG_H
#define QKXAUTHORIZECONFIRMDIALOG_H

#include <QDialog>

namespace Ui {
class QKxAuthorizeConfirmDialog;
}

class QKxAuthorizeConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QKxAuthorizeConfirmDialog(QWidget *parent = 0);
    ~QKxAuthorizeConfirmDialog();

private slots:
    void onAssignButtonClicked();
    void onCancelButtonClicked();
private:
    Ui::QKxAuthorizeConfirmDialog *ui;
};

#endif // QKXAUTHORIZECONFIRMDIALOG_H
