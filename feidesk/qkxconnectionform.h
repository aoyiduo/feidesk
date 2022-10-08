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

#ifndef QKXCONNECTIONFORM_H
#define QKXCONNECTIONFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxConnectionForm;
}

class QKxAbilityModel;
class QKxServiceManager;
class QKxConnectionForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxConnectionForm(QWidget *parent = 0);
    ~QKxConnectionForm();
    void setDeviceID(qint64 did);
private slots:
    void onAuthorizeButtonClicked();
    void onDeviceIDAction(int idx);
    void onDevicePasswordAction(int idx);
    void onAbilityItemClicked(const QModelIndex& idx);
    void onAbilityItemDbClicked(const QModelIndex& idx);
    void onLocalAbilityStartClicked();
    void onLocalAbilityStopClicked();    
    void onIDConnectClicked();
    void onIDConnectOptionClicked();
    void onIDConnectOptionMenuClicked();
    void onServiceFlagActive(quint64 flag);
    void syncAuthorize();

    //login
    void onAccountLoginResult(qint32 err, qint32 okcnt, const QString& msg);
    void onLogoutArrived();
private:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private:
    Ui::QKxConnectionForm *ui;
    QPointer<QKxServiceManager> m_serviceManager;
    qint64 m_deviceId;
};

#endif // QKXCONNECTIONFORM_H
