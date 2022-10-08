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

#ifndef QKXUSERCREATEDIALOG_H
#define QKXUSERCREATEDIALOG_H

#include <QDialog>

namespace Ui {
class QKxUserCreateDialog;
}

class QKxUserCreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QKxUserCreateDialog(QWidget *parent = 0);
    ~QKxUserCreateDialog();
private slots:
    void onButtonRegisterClicked();
    void onUserPasswordAction(int idx);
    void onLinkActivated(const QString& msg);
    void onRegisterUserResult(qint32 err, const QByteArray& msg);
private:
    Ui::QKxUserCreateDialog *ui;
};

#endif // QKXUSERCREATEDIALOG_H
