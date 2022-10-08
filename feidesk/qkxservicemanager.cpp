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

#include "qkxservicemanager.h"
#include "qtservice.h"
#include "qkxdeskdef.h"
#include "qkxutils.h"
#include "qkxftpserver.h"
#include "qkxfirewall.h"
#include "qkxnet_share.h"
#include "qkxsettingex.h"
#include "qkxwallpaper.h"
#include "qkxcapserver.h"
#include "qkxcapoption.h"
#include "qkxptyserver.h"

#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>

#define SERVICE_FTP_KEY             ("service/ftpRunning")
#define SERVICE_TERM_KEY            ("service/termRunning")
#define SERVICE_VNC_KEY             ("service/vncRunning")
#define SERVICE_CHAT_KEY             ("service/chatRunning")

#ifdef QT_DEBUG
#define VNC_PROXY_HOST        ("0.0.0.0")
#define VNC_PROXY_PORT        (6901)
#else
#define VNC_PROXY_HOST        ("127.0.0.1")
#define VNC_PROXY_PORT        (0)
#endif

QKxServiceManager::QKxServiceManager(QObject *parent)
    : QObject(parent)
    , m_serviceFlag(0)
{
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onCheckServiceStatus()));
    m_timer->setInterval(1000);
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
    QObject::connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
}

QKxServiceManager::~QKxServiceManager()
{
}

void QKxServiceManager::setDesktopActive(bool on)
{
    if(on) {        
        if(m_capServer != nullptr) {
            return;
        }
        QKxCapOption::instance()->setAuthorizePassword(VNC_SERVER_PASSWORD);
        m_capServer = new QKxCapServer("VNC_SERVER_URL", VNC_PROXY_HOST, VNC_PROXY_PORT);
        if(m_capServer->listenPort() > 0) {
            QKxSettingEx::setValue(SERVICE_VNC_KEY, true);
        }else{
            delete m_capServer;
        }
    }else if(m_capServer){
        delete m_capServer;
        QKxSettingEx::setValue(SERVICE_VNC_KEY, false);
    }

    checkServiceStatus();
}

void QKxServiceManager::setFtpActive(bool on)
{
    if(on) {
        if(m_ftpServer != nullptr) {
            return;
        }
        m_ftpServer = new QKxFtpServer("FTP_SERVER_URL");
        QKxSettingEx::setValue(SERVICE_FTP_KEY, true);
    }else if(m_ftpServer) {
        // because the thread bind by m_ftpServer has exit.
        // so delete it by manual.
        delete m_ftpServer;
        QKxSettingEx::setValue(SERVICE_FTP_KEY, false);
    }
    checkServiceStatus();
}

void QKxServiceManager::setTerminalActive(bool on)
{
    if(on) {
        if(m_ptyServer != nullptr) {
            return;
        }
        m_ptyServer = new QKxPtyServer("TERM_SERVER_URL");
        QKxSettingEx::setValue(SERVICE_TERM_KEY, true);
    }else if(m_ptyServer) {
        delete m_ptyServer;
        QKxSettingEx::setValue(SERVICE_TERM_KEY, false);
    }
    checkServiceStatus();
}

void QKxServiceManager::setChatActive(bool on)
{
    QKxSettingEx::setValue(SERVICE_CHAT_KEY, on);
    qputenv("CHAT_SERVER_URL", on ? "msg://local" : "");
    checkServiceStatus();
}

void QKxServiceManager::checkServiceStatus()
{
    m_countLeft = 5;
    m_timer->start(1000);
}

void QKxServiceManager::onAboutToQuit()
{
    if(m_capServer) {
        m_capServer->deleteLater();
    }
}

void QKxServiceManager::onCheckServiceStatus()
{
    m_countLeft--;
    if(m_countLeft <= 0) {
        m_timer->stop();
        return;
    }
    quint64 flag = 0;
    if(m_capServer && m_capServer->listenPort() > 0) {
        flag |= SERVICE_FLAG_VNC;
    }
    if(m_ftpServer) {
        flag |= SERVICE_FLAG_FTP;
    }
    if(m_ptyServer) {
        flag |= SERVICE_FLAG_TERMINAL;
    }

    bool isRunning = QKxSettingEx::value(SERVICE_CHAT_KEY, true).toBool();
    if(isRunning) {
        flag |= SERVICE_FLAG_CHAT;
    }

    if(flag != m_serviceFlag) {
        m_serviceFlag = flag;
        emit serviceReset(flag);
    }
}

void QKxServiceManager::init()
{
    {
        bool isRunning = QKxSettingEx::value(SERVICE_FTP_KEY, true).toBool();
        setFtpActive(isRunning);
    }
    {
        bool isRunning = QKxSettingEx::value(SERVICE_VNC_KEY, true).toBool();
        setDesktopActive(isRunning);
    }
    {
        bool isRunning = QKxSettingEx::value(SERVICE_TERM_KEY, true).toBool();
        setTerminalActive(isRunning);
    }
    {
        bool isRunning = QKxSettingEx::value(SERVICE_CHAT_KEY, true).toBool();
        setChatActive(isRunning);
    }

    checkServiceStatus();
}
