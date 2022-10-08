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

#ifndef QKXPOWERMAINWINDOW_H
#define QKXPOWERMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

namespace Ui {
class QKxPowerMainWindow;
}

class QAbstractButton;
class QKxConnectionForm;
class QKxMyDeviceForm;
class QKxOptionForm;
class QTimer;
class QLabel;

class QKxPowerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QKxPowerMainWindow(QWidget *parent = 0);
    ~QKxPowerMainWindow();
    void cancelAutoHide();
    static QKxMyDeviceForm *myDeviceForm();
    static QKxConnectionForm *connectionForm();
private slots:
    void onAccountLoginResult(qint32 err, qint32 okcnt, const QString& msg);
    void onAccountLogout();
    void onAnonymousConnect();
    void onNetworkStatusChecking();
    /* net module */
    void onIpcConnectedArrived();
    void onMacCheckFinishedArrived();
    void onServerConfigureInitializedArrived(bool ok, const QString& msg);
    void onP2pCheckFinishedArrived();
    void onNetworkReconnecting();
    void onNetStateArrived(int state, const QString& msg);
    /* menu */
    void onMenuHelpAboutOpen();
    void onMenuHelpVersionCheck();
    void onMenuWebsiteOpen();
    void onMenuManageDevice();
    void onMenuServerConfigure();
    void onMenuLanguageConfigure();
    void onMenuOptionsConfigure();
    void onMenuAccountExit();
    void onMenuInstallService();
    void onMenuUninstallService();
    void onMenuCleanAccount();

    /* control */
    void onButtonClicked(QAbstractButton* btn);
protected:
    void closeEvent(QCloseEvent *e);
private:
    Q_INVOKABLE void init();
    Q_INVOKABLE void updateServiceStatus();
    bool eventFilter(QObject *watched, QEvent *event);
    void initControlMenu();
    bool restoreLastState();
    void saveLastState();
    void syncNetInfo();
    void initMenuBar();
    void initStatusBar();
private:
    Ui::QKxPowerMainWindow *ui;
    QPointer<QKxOptionForm> m_option;
    QPointer<QKxMyDeviceForm> m_myDevice;
    QPointer<QKxConnectionForm> m_connection;
    QPointer<QAbstractButton> m_btnMyDevice;
    QPointer<QTimer> m_netCheck;
    QPointer<QLabel> m_netStatus;
    bool m_hasLogin, m_doAutoLogin;
    qint64 m_deviceId;
    qint64 m_netCheckCount;
    bool m_bCancelAutoHide;
};

#endif // QKXPOWERMAINWINDOW_H
