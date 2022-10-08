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

#include "qkxpowerapplication.h"
#include "qkxcap.h"
#include "qkxutils.h"
#include "qkxsettingex.h"
#include "qkxmydeviceform.h"
#include "qkxconnectionform.h"
#include "qkxoptionform.h"
#include "qkxpowermainwindow.h"
#include "qkxdesktopplaywindow.h"
#include "qkxfiletransferwindow.h"
#include "qtservice.h"
#include "qkxdeskdef.h"
#include "qkxaccount.h"
#include "qkxnetclient.h"
#include "qkxminiworkspacewidget.h"
#include "qkxcapserver.h"
#include "qkxtermwindow.h"
#include "qkxchatwindow.h"

#include <QDebug>
#include <QProcess>
#include <QCryptographicHash>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QMenu>

QKxPowerApplication::QKxPowerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_deviceId(0)
{
    setAttribute(Qt::AA_DontUseNativeMenuBar);
}

QKxPowerApplication::~QKxPowerApplication()
{

}

QKxPowerApplication *QKxPowerApplication::instance()
{
    return qobject_cast<QKxPowerApplication*>(QCoreApplication::instance());
}

QKxPowerMainWindow *QKxPowerApplication::mainWindow()
{
    QKxPowerApplication *app = qobject_cast<QKxPowerApplication*>(QCoreApplication::instance());
    return app->m_main;
}

QKxNetClient *QKxPowerApplication::netClient()
{
    return instance()->m_net;
}

void QKxPowerApplication::restart()
{
    qputenv("RUN_ACTION_NAME", "restart");
    QProcess::startDetached(applicationFilePath());
    QCoreApplication::quit();
}

qint64 QKxPowerApplication::deviceId()
{
    if(m_deviceId <= 0) {
        m_deviceId = m_net->deviceID();
    }
    return m_deviceId;
}

void QKxPowerApplication::closeAllRemoteProcess()
{
    for(auto it = m_remotes.begin(); it != m_remotes.end(); it++) {
        QProcess *proc = *it;
        proc->kill();
    }
}

void QKxPowerApplication::onIpcConnectedArrived()
{
    qDebug() << "onIpcConnectedArrived.";
}

void QKxPowerApplication::onMacCheckFinishedArrived()
{
    qDebug() << "onMacCheckFinishedArrived.";
}

void QKxPowerApplication::onServerConfigureInitializedArrived(bool ok, const QString &msg)
{

}

void QKxPowerApplication::onP2pCheckFinishedArrived()
{
    qDebug() << "onP2pCheckFinishedArrived.";
}

void QKxPowerApplication::onNetStateArrived(int state, const QString &msg)
{
    qDebug() << "onNetStateArrived" << state << msg;
}

void QKxPowerApplication::onAccountLoginResult(qint32 err, qint32 okcnt, const QString &msg)
{
    qDebug() << "onAccountLoginResult" << err << msg;
}

void QKxPowerApplication::onAccountLogout()
{
    qDebug() << "onAccountLogout";
}

void QKxPowerApplication::onMessageReceived(const QString &message)
{
    if(message == "active") {
        onShowWindow();
    }
}

void QKxPowerApplication::onAppStart()
{
    QByteArray url = qgetenv("NETHOST_PROCESS_URL");
    m_net = new QKxNetClient(url, this);
    QObject::connect(m_net, SIGNAL(ipcConnectedArrived()), this, SLOT(onIpcConnectedArrived()));
    QObject::connect(m_net, SIGNAL(peerEstablished(qint64,QVariantMap)), this, SLOT(onPeerEstablished(qint64,QVariantMap)));
    QObject::connect(m_net, SIGNAL(peerServiceStart(qint64,qint8)), this, SLOT(onPeerServiceStart(qint64,qint8)));
    QObject::connect(m_net, SIGNAL(peerServiceEnd(qint64,qint8)), this, SLOT(onPeerServiceEnd(qint64,qint8)));
    QObject::connect(m_net, SIGNAL(peerDisconnected(qint64)), this, SLOT(onPeerDisconnected(qint64)));
    QObject::connect(m_net, SIGNAL(macCheckFinishedArrived()), this, SLOT(onMacCheckFinishedArrived()));
    QObject::connect(m_net, SIGNAL(serverConfigureInitializedArrived(bool,QString)), this, SLOT(onServerConfigureInitializedArrived(bool,QString)));
    QObject::connect(m_net, SIGNAL(p2pCheckFinishedArrived()), this, SLOT(onP2pCheckFinishedArrived()));
    QObject::connect(m_net, SIGNAL(exitArrived()), this, SLOT(onServiceExitArrived()));
    QObject::connect(m_net, SIGNAL(broadcastMessageArrived(qint32,qint32,QVariant)), this, SLOT(onBroadcastMessageArrived(qint32,qint32,QVariant)));

    QMenu *menu = new QMenu();
    menu->addAction(QIcon(":/resource/skin/server.png"), QObject::tr("Show Window"), this, SLOT(onShowWindow()));
    menu->addAction(QIcon(":/resource/skin/exit.png"), QObject::tr("Exit"), this, SLOT(onAboutToExit()));

    m_tray.setIcon(QIcon(":/resource/skin/feidesk2.png"));
    m_tray.setToolTip(QObject::tr("FeiDesk"));
    m_tray.setContextMenu(menu);
    m_tray.show();
    QObject::connect(menu, SIGNAL(aboutToShow()), this, SLOT(onMenuAboutToShow()));
    QObject::connect(&m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayActive(QSystemTrayIcon::ActivationReason)));

    m_main = new QKxPowerMainWindow(nullptr);
    QObject::connect(m_net, SIGNAL(ipcConnectedArrived()), m_main, SLOT(onIpcConnectedArrived()));
    QObject::connect(m_net, SIGNAL(networkReconnecting()), m_main, SLOT(onNetworkReconnecting()));
    QObject::connect(m_net, SIGNAL(macCheckFinishedArrived()), m_main, SLOT(onMacCheckFinishedArrived()));
    QObject::connect(m_net, SIGNAL(serverConfigureInitializedArrived(bool,QString)), m_main, SLOT(onServerConfigureInitializedArrived(bool,QString)));
    QObject::connect(m_net, SIGNAL(p2pCheckFinishedArrived()), m_main, SLOT(onP2pCheckFinishedArrived()));
    m_main->show();
}

void QKxPowerApplication::onServiceExitArrived()
{
    quit();
}

void QKxPowerApplication::onBroadcastMessageArrived(int target, int event, const QVariant &v)
{
    qDebug() << "target" << target << "event" << event << v.typeName();
    if(target == MESSAGE_TARGET_HISTORY_MODEL) {
        if(event == 1) {
            QMap<QString,QVariant> data = v.toMap();
            qint64 did = data.value("deviceId").toLongLong();
            int type = data.value("type").toInt();
            QString why = data.value("why").toString();
            qDebug() << "onMessageArrived" << did << type << why;
        }
    }
}

void QKxPowerApplication::onPeerServiceStart(qint64 did, qint8 type)
{
    qDebug() << "onPeerServiceStart" << did << type;
    if(m_workspace) {
        m_workspace->addService(did, type);
    }
    if(type == 3) {
        responeRemoteChat(did);
    }
}

void QKxPowerApplication::onPeerServiceEnd(qint64 did, qint8 type)
{
    qDebug() << "onPeerServiceEnd" << did << type;
    if(m_workspace) {
        m_workspace->removeService(did, type);
    }
}

void QKxPowerApplication::onPeerEstablished(qint64 did, const QVariantMap &ext)
{
    qDebug() << "onPeerEstablished" << did << ext;
    if(m_workspace == nullptr) {
        m_workspace = new QKxMiniWorkspaceWidget(nullptr);
        m_workspace->resize(320,240);
        m_workspace->show();
    }
    m_workspace->addSession(did, ext);
}

void QKxPowerApplication::onPeerDisconnected(qint64 did)
{
    qDebug() << "onPeerDisconnected" << did;
    if(m_workspace) {
        m_workspace->removeSession(did);
        if(m_workspace->sessionEmpty()) {
            m_workspace->deleteLater();
            m_workspace = nullptr;
        }
    }
}

void QKxPowerApplication::onTrayActive(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Context || reason == QSystemTrayIcon::Trigger) {
        onShowWindow();
    }
}

void QKxPowerApplication::onMenuAboutToShow()
{

}

void QKxPowerApplication::onShowWindow()
{
    m_main->cancelAutoHide();
    m_main->show();
    m_main->raise();
    m_main->activateWindow();
}

void QKxPowerApplication::onAboutToExit()
{
#ifdef Q_OS_WIN
    m_tray.hide();
    if(QKxCapServer::isRunAsService()) {
        QtServiceController sc(SERVICE_NAME);
        if(sc.isRunning()) {
            sc.sendCommand(SERVICE_COMMAND_CODE_EXIT);
        }
    }
#endif
    qDebug() << "onAboutToExit";
    quit();
}

void QKxPowerApplication::onRemoteProcessFinished()
{
    QProcess *proc = qobject_cast<QProcess*>(sender());
    m_remotes.removeOne(proc);
}

void QKxPowerApplication::openRemoteService(qint64 did, int type, const QByteArray &friendlyName, const QByteArray &password, bool byAccount)
{
    if(type == 1) {
        // desktop
        openRemoteDesktop(did, friendlyName, password, byAccount);
    }else if(type == 2) {
        // ftp
        openRemoteFileTransfer(did, friendlyName, password, byAccount);
    }else if(type == 3) {
        // chat.
        QMap<QByteArray,QByteArray> env;
        openRemoteChat(did, friendlyName, password, byAccount, env);
    }else if(type == 4) {
        openRemoteTerminal(did, friendlyName, password, byAccount);
    }
}

void QKxPowerApplication::openRemoteDesktop(qint64 did, const QByteArray &friendlyName, const QByteArray &password, bool byAccount)
{
    qDebug() << "openRemoteDesktop" << did;
    bool isInner = false;
    if(isInner) {        
        qputenv("LOCAL_FRIENDLY_NAME", friendlyName);
        qputenv("REMOTE_DEVICE_ID", QByteArray::number(did));
        qputenv("REMOTE_DEVICE_TOKEN", password.toBase64());
        qputenv("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        QKxDesktopPlayWindow *form = new QKxDesktopPlayWindow(nullptr);
        QObject::connect(form, SIGNAL(closeArrived()), form, SLOT(deleteLater()));
        form->resize(1024, 768);
        form->show();
    }else{
        QProcess* proc = new QProcess(this);
        m_remotes.append(proc);
        QObject::connect(proc, SIGNAL(finished(int)), this, SLOT(onRemoteProcessFinished()));
        QObject::connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LOCAL_FRIENDLY_NAME", friendlyName);
        env.insert("REMOTE_DEVICE_ID", QByteArray::number(did));
        env.insert("REMOTE_DEVICE_TOKEN", password.toBase64());
        env.insert("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        env.insert("RUN_ACTION_NAME", "desk:" + QString("%1").arg(applicationPid()).toLatin1());
        proc->setProcessEnvironment(env);
        proc->setProgram(QCoreApplication::applicationFilePath());
        proc->start();
    }
}

void QKxPowerApplication::openRemoteFileTransfer(qint64 did, const QByteArray &friendlyName, const QByteArray &password, bool byAccount)
{
    qDebug() << "openRemoteFileTransfer" << did;
    bool isInner = false;
    if(isInner) {
        qputenv("LOCAL_FRIENDLY_NAME", friendlyName);
        qputenv("REMOTE_DEVICE_ID", QByteArray::number(did));
        qputenv("REMOTE_DEVICE_TOKEN", password.toBase64());
        qputenv("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        QKxFileTransferWindow *form = new QKxFileTransferWindow(nullptr);
        QObject::connect(form, SIGNAL(closeArrived()), form, SLOT(deleteLater()));
        form->resize(800, 640);
        form->show();
    }else{
        QProcess* proc = new QProcess(this);
        m_remotes.append(proc);
        QObject::connect(proc, SIGNAL(finished(int)), this, SLOT(onRemoteProcessFinished()));
        QObject::connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LOCAL_FRIENDLY_NAME", friendlyName);
        env.insert("REMOTE_DEVICE_ID", QByteArray::number(did));
        env.insert("REMOTE_DEVICE_TOKEN", password.toBase64());
        env.insert("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        env.insert("RUN_ACTION_NAME", "ftp:" + QString("%1").arg(applicationPid()).toLatin1());
        proc->setProcessEnvironment(env);
        proc->setProgram(QCoreApplication::applicationFilePath());
        proc->start();
    }
}

void QKxPowerApplication::openRemoteChat(qint64 did, const QByteArray &friendlyName, const QByteArray &password, bool byAccount, const QMap<QByteArray,QByteArray>& envs)
{
    qDebug() << "openRemoteChat" << did;
    bool isInner = false;
    if(isInner) {
        for(auto it = envs.begin(); it != envs.end(); it++) {
            qputenv(it.key(), it.value());
        }
        qputenv("LOCAL_FRIENDLY_NAME", friendlyName);
        qputenv("REMOTE_DEVICE_ID", QByteArray::number(did));
        qputenv("REMOTE_DEVICE_TOKEN", password.toBase64());
        qputenv("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        QKxChatWindow *form = new QKxChatWindow(nullptr);
        QObject::connect(form, SIGNAL(closeArrived()), form, SLOT(deleteLater()));
        form->resize(320, 480);
        form->show();
    }else{
        QProcess* proc = new QProcess(this);
        m_remotes.append(proc);
        QObject::connect(proc, SIGNAL(finished(int)), this, SLOT(onRemoteProcessFinished()));
        QObject::connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        for(auto it = envs.begin(); it != envs.end(); it++) {
            env.insert(it.key(), it.value());
        }
        env.insert("LOCAL_FRIENDLY_NAME", friendlyName);
        env.insert("REMOTE_DEVICE_ID", QByteArray::number(did));
        env.insert("REMOTE_DEVICE_TOKEN", password.toBase64());
        env.insert("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        env.insert("RUN_ACTION_NAME", "chat:" + QString("%1").arg(applicationPid()).toLatin1());
        proc->setProcessEnvironment(env);
        proc->setProgram(QCoreApplication::applicationFilePath());
        proc->start();
    }
}

void QKxPowerApplication::responeRemoteChat(qint64 did)
{
    QMap<QByteArray, QByteArray> envs;
    envs.insert("RUN_AS_SERVER", "1");
    openRemoteChat(did, QByteArray(), QByteArray(), false, envs);
}

void QKxPowerApplication::openRemoteTerminal(qint64 did, const QByteArray &friendlyName, const QByteArray& password, bool byAccount)
{
    qDebug() << "openRemoteTerminal" << did;
    bool isInner = false;
    if(isInner) {
        qputenv("LOCAL_FRIENDLY_NAME", friendlyName);
        qputenv("REMOTE_DEVICE_ID", QByteArray::number(did));
        qputenv("REMOTE_DEVICE_TOKEN", password.toBase64());
        qputenv("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        QKxTermWindow *form = new QKxTermWindow(nullptr);
        QObject::connect(form, SIGNAL(closeArrived()), form, SLOT(deleteLater()));
        form->resize(800, 640);
        form->show();
    }else{
        QProcess* proc = new QProcess(this);
        m_remotes.append(proc);
        QObject::connect(proc, SIGNAL(finished(int)), this, SLOT(onRemoteProcessFinished()));
        QObject::connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LOCAL_FRIENDLY_NAME", friendlyName);
        env.insert("REMOTE_DEVICE_ID", QByteArray::number(did));
        env.insert("REMOTE_DEVICE_TOKEN", password.toBase64());
        env.insert("SERVICE_ACCOUNT", byAccount ? "1" : "0");
        env.insert("RUN_ACTION_NAME", "term:" + QString("%1").arg(applicationPid()).toLatin1());
        proc->setProcessEnvironment(env);
        proc->setProgram(QCoreApplication::applicationFilePath());
        proc->start();
    }
}
