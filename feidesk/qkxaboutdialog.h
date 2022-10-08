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

#ifndef QKXABOUTDIALOG_H
#define QKXABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class QKxAboutDialog;
}

class QKxAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QKxAboutDialog(QWidget *parent = 0);
    ~QKxAboutDialog();

private slots:
    void onLinkActivated(const QString& link);
private:
    Ui::QKxAboutDialog *ui;
};

#endif // QKXABOUTDIALOG_H
