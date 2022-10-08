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

#ifndef QKXDESKTOPPLAYWINDOW_H
#define QKXDESKTOPPLAYWINDOW_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxDesktopPlayWindow;
}

class QKxDesktopPlayForm;
class QKxDesktopToolForm;
class QKxDesktopFtpWidget;
class QScrollArea;
class QPropertyAnimation;
class QKxNetSimpleClient;
class QKxDesktopPlayStatusForm;
class QTimer;
class QKxDesktopPlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QKxDesktopPlayWindow(QWidget *parent = 0);
    ~QKxDesktopPlayWindow();

signals:
    void closeArrived();
private slots:
    // net module
    void onSnapshotArrived(qint64 did, int type);
    void onTryAgainLater();
    void onIpcConnectedArrived();
    void onNetStateArrived(int state, const QString& msg);
    void onServiceExitArrived();
    void onBroadcastMessageArrived(qint32 target, qint32 event, const QVariant& v);
    void onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant & v, const QVariant & data);
    void onPeerMessageArrived(qint32 type, const QByteArray &msg);
    // self
    void onToolEnter();
    void onToolLeave();
    void onDeleteLater();
    void onToolSizeChanged(const QSize& sz);
    void onConnectTimeout();
    // mutex message.
    void onMessageReceived(const QString &msg);
    void onFtpClicked();
private:
    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *e);
    void updateToolbarPosition();
private:
    qint64 m_deviceId;
    QByteArray m_password;
    QByteArray m_friendlyName;
    bool m_fromAccount;
    bool m_isPasswordRight;
    bool m_reconnected;
    int m_mode;

    Ui::QKxDesktopPlayWindow *ui;
    QPointer<QScrollArea> m_area;
    QPointer<QKxDesktopPlayForm> m_player;
    QPointer<QKxDesktopToolForm> m_tool;
    QPointer<QPropertyAnimation> m_ani;
    QPointer<QKxDesktopFtpWidget> m_ftp;

    QPointer<QKxNetSimpleClient> m_net;
    QPointer<QKxDesktopPlayStatusForm> m_status;
    QPointer<QTimer> m_timer;
};



#endif // QKXDESKTOPPLAYWINDOW_H
