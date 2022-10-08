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

#include "qkxusercreatedialog.h"
#include "ui_qkxusercreatedialog.h"
#include "qkxutils.h"
#include "qkxbuttonassist.h"
#include "qkxnetclient.h"
#include "qkxpowerapplication.h"

#include <QTimer>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

QKxUserCreateDialog::QKxUserCreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxUserCreateDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("User Register"));

    QObject::connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(onButtonRegisterClicked()));

    ui->password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    QKxButtonAssist *assist = new QKxButtonAssist(":/resource/skin/eye.png", ui->password);
    QObject::connect(assist, SIGNAL(clicked(int)), this, SLOT(onUserPasswordAction(int)));


    QKxNetClient *net = QKxPowerApplication::netClient();
    QObject::connect(net, SIGNAL(registerUserResult(qint32,QByteArray)), this, SLOT(onRegisterUserResult(qint32,QByteArray)));

    QTimer::singleShot(0, this, [=]{
        QSize sz = minimumSize();
        resize(sz);
    });
}

QKxUserCreateDialog::~QKxUserCreateDialog()
{
    delete ui;
}

void QKxUserCreateDialog::onButtonRegisterClicked()
{
    QString name = ui->loginName->text();
    if(name.isEmpty()) {
        QMessageBox::warning(this, tr("Tip"), tr("The login name should not be empty."));
        return;
    }
    if(!QKxUtils::checkEmailOrPhone(name)) {
        QMessageBox::warning(this, tr("Tip"), tr("login name should be a valid telephone number or email."));
        return;
    }

    QString nickName = ui->nickName->text();
    if(nickName.isEmpty()) {
        QMessageBox::warning(this, tr("Tip"), tr("The nickname should not be empty."));
        return;
    }

    QString pwd = ui->password->text();
    if(pwd.isEmpty()) {
        QMessageBox::warning(this, tr("Tip"), tr("The password should not be empty."));
        return;
    }
    if(pwd.length() < 6) {
        QMessageBox::warning(this, tr("Tip"), tr("The number of password characters needs to be more than six"));
        return;
    }
    QString pwdMd5 = QCryptographicHash::hash(pwd.toLatin1(), QCryptographicHash::Md5).toHex();
    QKxNetClient *net = QKxPowerApplication::netClient();
    net->registerUser(name.toLatin1(), nickName.toUtf8(), pwdMd5.toUtf8(), QVariantMap());
}

void QKxUserCreateDialog::onUserPasswordAction(int idx)
{
    if(QLineEdit::Normal == ui->password->echoMode()) {
        ui->password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    }else{
        ui->password->setEchoMode(QLineEdit::Normal);
    }
}

void QKxUserCreateDialog::onLinkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void QKxUserCreateDialog::onRegisterUserResult(qint32 err, const QByteArray &msg)
{
    qDebug() << err << msg;
    if(err == 0) {
        QMessageBox::information(this, tr("registry result"), tr("success to register"));
        done(Accepted);
    }else{
        QMessageBox::information(this, tr("registry result"), tr("failed to register:") + msg);
    }
}
