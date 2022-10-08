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

#ifndef QKXCHATWINDOW_H
#define QKXCHATWINDOW_H

#include "qkxchatinteraction.h"

#include <QWidget>
#include <QPointer>


namespace Ui {
class QKxChatWindow;
}

class QKxNetSimpleClient;
class QKxChatStatusForm;
class QTimer;
class QKxTextBrowser;
class QKxChatInteraction;

class QKxChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QKxChatWindow(QWidget *parent = nullptr);
    ~QKxChatWindow();
signals:
    void closeArrived();
    void peerConnectedArrived(const QString& addr);
private slots:
    // net module
    void onConnectTimeout();
    void onTryAgainLater();
    void onIpcConnectedArrived();

    void onNetStateArrived(int state, const QString& msg);
    void onServiceExitArrived();
    void onBroadcastMessageArrived(qint32 target, qint32 event, const QVariant& v);
    void onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant & v, const QVariant & data);
    void onPeerConnectedArrived(const QString& addr);
    void onPeerMessageArrived(qint32 type, const QByteArray& msg);

    void onMessageReceived(const QString& msg);
    void onChatMessageArrived(const QString& msg);
    void onChatStateChanged(QKxChatInteraction::EState state);

    void onConnectedArrived();
    void onDisconnectedArrived();

    void onButtonSendClicked();
private:
    virtual void resizeEvent(QResizeEvent *ev);
private:
    Ui::QKxChatWindow *ui;
    QPointer<QKxChatStatusForm> m_status;
    QPointer<QKxTextBrowser> m_history;
    QPointer<QTimer> m_timer;
    int m_mode; // 0: auto, 1: turn, 2: stun.
    bool m_runAsServer;

    qint64 m_timeLast;

    qint64 m_deviceId, m_localDeviceId;
    QByteArray m_friendlyName;
    QByteArray m_password;
    bool m_fromAccount;
    bool m_isPasswordRight;

    bool m_reconnected;
    QPointer<QKxNetSimpleClient> m_net;    
    QPointer<QKxChatInteraction> m_chat;
};

#endif // QKXCHATWINDOW_H
