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

#ifndef QKXPOWERAPPLICATION_H
#define QKXPOWERAPPLICATION_H

#include <QList>
#include <QPointer>
#include <QApplication>
#include <QSystemTrayIcon>

class QKxNet;
class QKxCap;
class QKxPowerMainWindow;
class QKxNetClient;
class QAbstractItemModel;
class QKxMiniWorkspaceWidget;
class QProcess;

class QKxPowerApplication : public QApplication
{
    Q_OBJECT
public:
    explicit QKxPowerApplication(int &argc, char **argv);
    ~QKxPowerApplication();

    static QKxPowerApplication *instance();
    static QKxPowerMainWindow *mainWindow();
    static QKxNetClient *netClient();
    static void restart();

    qint64 deviceId();
    void closeAllRemoteProcess();

private slots:
    // net module
    void onIpcConnectedArrived();
    void onMacCheckFinishedArrived();
    void onServerConfigureInitializedArrived(bool ok, const QString& msg);
    void onP2pCheckFinishedArrived();
    void onNetStateArrived(int state, const QString& msg);
    void onServiceExitArrived();
    void onBroadcastMessageArrived(int target, int event, const QVariant& v);

    // peer
    void onPeerServiceStart(qint64 did, qint8 type);
    void onPeerServiceEnd(qint64 did, qint8 type);
    void onPeerEstablished(qint64 did, const QVariantMap& ext);
    void onPeerDisconnected(qint64 did);
    // account module
    void onAccountLoginResult(qint32 err, qint32 okcnt, const QString& msg);
    void onAccountLogout();
    // on message
    void onMessageReceived(const QString &message);
    void onAppStart();
    // tray
    void onTrayActive(QSystemTrayIcon::ActivationReason reason);
    // window
    void onMenuAboutToShow();
    void onShowWindow();
    void onAboutToExit();

    void onRemoteProcessFinished();
private:
    Q_INVOKABLE void openRemoteService(qint64 did, int type, const QByteArray& friendlyName, const QByteArray& password, bool byAccount);
    void openRemoteDesktop(qint64 did, const QByteArray& friendlyName, const QByteArray& password, bool byAccount);
    void openRemoteFileTransfer(qint64 did, const QByteArray& friendlyName, const QByteArray& password, bool byAccount);
    void openRemoteChat(qint64 did, const QByteArray& friendlyName, const QByteArray& password, bool byAccount, const QMap<QByteArray,QByteArray>& env);
    void responeRemoteChat(qint64 did);
    void openRemoteTerminal(qint64 did, const QByteArray& friendlyName, const QByteArray& password, bool byAccount);
private:
    QPointer<QKxPowerMainWindow> m_main;
    QPointer<QKxMiniWorkspaceWidget> m_workspace;
    QPointer<QKxNetClient> m_net;
    QList<QPointer<QProcess>> m_remotes;
    QSystemTrayIcon m_tray;
    qint64 m_deviceId;
};

#endif // QKXPOWERAPPLICATION_H
