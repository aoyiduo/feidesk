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

#include "qkxauthorizeconfirmdialog.h"
#include "ui_qkxauthorizeconfirmdialog.h"
#include "qkxmydeviceform.h"
#include "qkxpowermainwindow.h"
#include "qkxaccount.h"
#include "qkxsettingex.h"

#include <QTimer>
#include <QMessageBox>

QKxAuthorizeConfirmDialog::QKxAuthorizeConfirmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxAuthorizeConfirmDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Authorize Configure"));

    QObject::connect(ui->btnAssign, SIGNAL(clicked(bool)), this, SLOT(onAssignButtonClicked()));
    QObject::connect(ui->btnCancel, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));


    QKxAccount *account = QKxMyDeviceForm::loginAccount();
    if(account != nullptr && account->hasLogin()) {
        ui->loginName->setText(account->loginName());
    }else{
        ui->error->setText(tr("Please login first."));
    }

    QTimer::singleShot(0, this, [=]{
        QSize sz = minimumSize();
        resize(sz);
    });
}

QKxAuthorizeConfirmDialog::~QKxAuthorizeConfirmDialog()
{
    delete ui;
}

void QKxAuthorizeConfirmDialog::onAssignButtonClicked()
{
    QString name = ui->loginName->text();
    if(name.isEmpty()) {
        QMessageBox::information(this, tr("parameter"), tr("login name is empty."));
        return;
    }
    QKxSettingEx::setAuthorizeLoginName(name);
    done(Accepted);
}

void QKxAuthorizeConfirmDialog::onCancelButtonClicked()
{
    done(Rejected);
}
