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

#include "qkxsettingex.h"
#include "qkxutils.h"
#include "qkxdeskdef.h"

#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QProcess>
#include <QTranslator>
#include <QTcpServer>
#include <QUuid>
#include <QDateTime>
#include <QDebug>
#include <QCryptographicHash>


void QKxSettingEx::checkFirewall()
{
    QString path = QKxUtils::applicationFilePath();
    QString appPath = value("lastAppPath", "").toString();
    qDebug() << "checkFirewall" << path << appPath;
    QTcpServer hit;
    hit.listen();
}

qint64 QKxSettingEx::lastDeviceID()
{
    return value("lastDeviceID", 0).toLongLong();
}

void QKxSettingEx::setLastDeviceID(qint64 uid)
{
    setValue("lastDeviceID", uid);
}

QString QKxSettingEx::lastUserName()
{
    return value("userName", "").toString();
}

void QKxSettingEx::setLastUserName(const QString &name)
{
    setValue("userName", name);
}

QString QKxSettingEx::lastUserPassword()
{
    QString pwd = value("userPassword", "").toString();
    return pwd;
}

void QKxSettingEx::setLastUserPassword(const QString &pwd)
{
    setValue("userPassword", pwd);
}

bool QKxSettingEx::rememberAccount()
{
    return value("rememberAccount", false).toBool();
}

void QKxSettingEx::setRememberAccount(bool on)
{
    setValue("rememberAccount", on);
}

bool QKxSettingEx::autoLogin()
{
    return value("autoLogin", false).toBool();
}

void QKxSettingEx::setAutoLogin(bool on)
{
    setValue("autoLogin", on);
}

QString QKxSettingEx::authorizeLoginName()
{
    return value("authorizeLoginName", "").toString();
}

void QKxSettingEx::setAuthorizeLoginName(const QString &name)
{
    setValue("authorizeLoginName", name);
}

void QKxSettingEx::cleanAutoLogin()
{
    remove("lastDeviceID");
    remove("authorizeLoginName");
    remove("userName");
    remove("userPassword");
    remove("rememberAccount");
    remove("autoLogin");
    sync();
}

QString QKxSettingEx::languageName()
{
    QString name = QKxSettingEx::value("language/path", "").toString();
    if(name.isEmpty()) {
        QLocale locale;
        if(locale.language() == QLocale::Chinese) {
            return "Chinese";
        }
        return "English";
    }
    if(name == "Chinese") {
        return "Chinese";
    }
    return "English";
}

QString QKxSettingEx::languageFile()
{
    if(languageName() == "Chinese") {
        return ":/language/feidesk_zh.qm";
    }
    return ":/language/feidesk_en.qm";
}

void QKxSettingEx::setLanguageName(const QString &name)
{
    setValue("language/path", name);
}

int QKxSettingEx::networkConnection()
{
    return value("network/connection", 1).toInt();
}

void QKxSettingEx::setNetworkConnection(int m)
{
    setValue("network/connection", m);
}

int QKxSettingEx::networkP2PProirity()
{
    return value("network/p2p", 1).toInt();
}

void QKxSettingEx::setNetworkP2PProirity(int m)
{
    setValue("network/p2p", m);
}

bool QKxSettingEx::desktopLocalMouseShow()
{
    return value("desktop/localMouse", false).toBool();
}

void QKxSettingEx::setDesktopLocalMouseShow(bool on)
{
    setValue("desktop/localMouse", on);
}

QString QKxSettingEx::deviceManageWebsite()
{
    QString addr = deviceLoginAddress();
    QStringList hp = addr.split(':');
    QString host = hp.at(0);
    // all address defautl 80,
    // if 443 should auto jump on website.
    return QString("http://%1").arg(host);
}

QString QKxSettingEx::deviceLoginAddress()
{
    QString addr = value("deviceLogin", "").toString();
    if(addr.isEmpty()) {
        return officeDeviceLoginAddress();
    }
    return addr;
}

QString QKxSettingEx::officeDeviceLoginAddress()
{
    return QString("%1:%2").arg(MY_DEVICE_MANAGE_HOST).arg(MY_DEVICE_LOGIN_PORT);
}

void QKxSettingEx::setDeviceLoginAddress(const QString &addr)
{
    setValue("deviceLogin", addr);
}
