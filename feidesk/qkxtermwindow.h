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

#ifndef QKXTERMWINDOW_H
#define QKXTERMWINDOW_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxTermWindow;
}

class QKxTermWidgetEx;
class QKxPtyRequest;
class QKxEchoInput;
class QKxNetSimpleClient;
class QKxTermStatusForm;
class QTimer;

class QKxTermWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QKxTermWindow(QWidget *parent = nullptr);
    ~QKxTermWindow();

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
    void onPeerMessageArrived(qint32 type, const QByteArray& msg);

    void onMessageReceived(const QString& msg);

    void onConnectedArrived();
    void onDisconnectedArrived();
private:
    virtual void resizeEvent(QResizeEvent *ev);
private:
    Ui::QKxTermWindow *ui;
    QPointer<QKxTermWidgetEx> m_term;
    QPointer<QKxTermStatusForm> m_status;
    QPointer<QTimer> m_timer;
    int m_mode; // 0: auto, 1: turn, 2: stun.

    qint64 m_deviceId;
    QByteArray m_friendlyName;
    QByteArray m_password;
    bool m_fromAccount;
    bool m_isPasswordRight;

    bool m_reconnected;
    QPointer<QKxNetSimpleClient> m_net;
};

#endif // QKXTERMWINDOW_H
