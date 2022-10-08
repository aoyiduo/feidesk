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

#include "qkxserverdialog.h"
#include "ui_qkxserverdialog.h"

#include "qkxsettingex.h"

#include <QTimer>
#include <QUrl>
#include <QMessageBox>
#include <QIntValidator>

QKxServerDialog::QKxServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxServerDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Server Address"));

    QString addr = QKxSettingEx::deviceLoginAddress();
    QStringList hp = addr.split(':');
    ui->host->setText(hp.at(0));
    ui->port->setText(hp.at(1));
    ui->port->setValidator(new QIntValidator(22, 65535, this));

    QObject::connect(ui->btnRestore, SIGNAL(clicked(bool)), this, SLOT(onRestoreButtonClicked()));
    QObject::connect(ui->btnSave, SIGNAL(clicked(bool)), this, SLOT(onSaveButtonClicked()));
    setMinimumWidth(250);
    QTimer::singleShot(0, this, [=]{
        QSize sz = minimumSize();
        resize(sz);
    });
}

QKxServerDialog::~QKxServerDialog()
{
    delete ui;
}

void QKxServerDialog::onRestoreButtonClicked()
{
    QString addr = QKxSettingEx::officeDeviceLoginAddress();
    QStringList hp = addr.split(':');
    ui->host->setText(hp.at(0));
    ui->port->setText(hp.at(1));
}

void QKxServerDialog::onSaveButtonClicked()
{
    QString host = ui->host->text();
    if(host.startsWith("http")) {
        QMessageBox::information(this, tr("input error"), tr("the host should not start with http or https etc."));
        return;
    }
    QString port = ui->port->text();
    QString hp = host + ":" + port;
    QString addr = QKxSettingEx::deviceLoginAddress();
    QKxSettingEx::setDeviceLoginAddress(hp);
    if(hp != addr) {
        QKxSettingEx::cleanAutoLogin();
    }
    done(hp == addr ? Rejected : Accepted);
}
