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

#ifndef QKXOPTIONSDIALOG_H
#define QKXOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class QKxOptionsDialog;
}

class QKxOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    enum EChangeFlag{
        ECF_Connection = 1,
        ECF_P2PMode = 1 << 1,
    };
    Q_FLAG(EChangeFlag)
public:
    explicit QKxOptionsDialog(QWidget *parent = nullptr);
    ~QKxOptionsDialog();
    QFlags<QKxOptionsDialog::EChangeFlag> result();
private:
    void initDefault();
private slots:
    void onButtonSaveClicked();
    void onTreeItemClicked(const QModelIndex& index);
private:
    Ui::QKxOptionsDialog *ui;
    QFlags<QKxOptionsDialog::EChangeFlag> m_flags;
};

#endif // QKXOPTIONSDIALOG_H
