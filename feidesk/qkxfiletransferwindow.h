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

#ifndef QKXFILETRANSFERWINDOW_H
#define QKXFILETRANSFERWINDOW_H

#include <QWidget>
#include <QPointer>


class QKxNetSimpleClient;
class QTimer;
class QKxFtpRequest;
class QKxFtpTransferWidget;
class QKxFileTransferStatusForm;
class QPushButton;

class QKxFileTransferWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QKxFileTransferWindow(QWidget *parent = 0);
    ~QKxFileTransferWindow();

signals:
    void closeArrived();
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
protected:
    virtual void resizeEvent(QResizeEvent *ev);
private:
    void reconnect();
    bool listFiles(const QString& path, QStringList& all, int maxCount = 50);
private:
    QString m_pathTask;
    qint64 m_deviceId;
    QByteArray m_friendlyName;
    QByteArray m_password;
    bool m_fromAccount;
    bool m_isPasswordRight;
    int m_mode; // 0: auto, 1: turn, 2: stun.

    QPointer<QKxFtpTransferWidget> m_transfer;
    QPointer<QKxFtpRequest> m_ftpRequest;
    QPointer<QKxNetSimpleClient> m_net;
    QPointer<QTimer> m_timer;
    QPointer<QKxFileTransferStatusForm> m_status;
    QString m_serverUrl;
    bool m_reconnected;
};

#endif // QKXFILETRANSFERWINDOW_H
