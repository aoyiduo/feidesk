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

#pragma once

#include "qkxsetting.h"

#include <QVariant>
#include <QMap>

class QKxSettingEx : public QKxSetting
{
public:
    static void checkFirewall();
    static qint64 lastDeviceID();
    static void setLastDeviceID(qint64 uid);
    static QString lastUserName();
    static void setLastUserName(const QString& name);
    static QString lastUserPassword();
    static void setLastUserPassword(const QString& pwd);
    static bool rememberAccount();
    static void setRememberAccount(bool on);
    static bool autoLogin();
    static void setAutoLogin(bool on);
    static QString authorizeLoginName();
    static void setAuthorizeLoginName(const QString& name);
    static void cleanAutoLogin();

    static QString officeDeviceLoginAddress();
    static QString deviceManageWebsite();
    static QString deviceLoginAddress();
    static void setDeviceLoginAddress(const QString& addr);

    static QString languageName();
    static QString languageFile();
    static void setLanguageName(const QString& name);

    // options - network
    // 0: tcp ; 1: udp first or else tcp.
    static int networkConnection();
    static void setNetworkConnection(int m);
    // 0: turn; 1: stun first or else turn.
    static int networkP2PProirity();
    static void setNetworkP2PProirity(int m);
    // options - desktop
    static bool desktopLocalMouseShow();
    static void setDesktopLocalMouseShow(bool on);


};
