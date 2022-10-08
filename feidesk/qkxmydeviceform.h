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

#ifndef QKXMYDEVICEFORM_H
#define QKXMYDEVICEFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxMyDeviceForm;
}

class QMenu;
class QKxButtonAssist;
class QTimer;
class QKxAccount;
class QKxDeviceTreeModel;
class QKxAbilityModel;
class QKxServiceManager;
class QKxRemoteAbilityModel;

class QKxMyDeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxMyDeviceForm(QWidget *parent = 0);
    ~QKxMyDeviceForm();
    static QKxAccount *loginAccount();
    void logout();
    void cleanAccount();
signals:
    void loginResult(qint32 err, qint32 okcnt, const QString& msg);
    void logoutArrived();
private slots:
    void onTreeHiddenArrived(int row, bool on);
    void onTreeDeviceExpandAll();
    void onUserPasswordAction(int idx);
    void onAutoSetPasswordMode();
    void onUserPasswordChanged(const QString& txt);
    void onTryToLogin();
    void onCheckLoginButtonClicked();

    // account.
    void onAccountLinkActivated(const QString& link);
    void onAccountToRegister();
    void onAccountPeerError(qint32 err, const QString& msg);
    void onAccountConnectionError(qint32 err, const QString& msg);
    void onAccountLoginResult(qint32 err, qint32 okcnt, const QString& msg);
    void onRemoteFileTransfer();
    void onRemoteTerminalOpen();
    void onRemoteConnectionCheck();
    void onRemoteDesktopOpen();
    void onAutoStartLogin();
    //action.
    void onRemoteAbilityDoubleClicked(const QModelIndex &idx);
    void onRemoteAbilityConnectClicked();
    //device.
    void onDeviceListItemClicked(const QModelIndex& idx);
    // reconnecting.
    void onReconnecting();
private:
    bool eventFilter(QObject *watched, QEvent *event);
    void startLogin(const QString &name, const QString &pwd);
    void exitLogin();
    void selectFirstChild();
private:
    Ui::QKxMyDeviceForm *ui;
    QPointer<QKxAccount> m_account;
    QPointer<QTimer> m_pwdTimer;
    qint64 m_deviceId;
    QPointer<QKxButtonAssist> m_passVisible;
    QPointer<QMenu> m_menu;
    QPointer<QKxDeviceTreeModel> m_deviceModel;
    QPointer<QKxRemoteAbilityModel> m_remoteModel;
};

#endif // QKXMYDEVICEFORM_H
