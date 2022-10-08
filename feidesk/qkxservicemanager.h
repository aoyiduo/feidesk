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

#ifndef QKXSERVICMANAGER_H
#define QKXSERVICMANAGER_H

#include <QPointer>
#include <QObject>

class QKxFtpServer;
class QKxCapServer;
class QKxPtyServer;
class QTimer;
class QKxServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit QKxServiceManager(QObject *parent = nullptr);
    ~QKxServiceManager();
    void setDesktopActive(bool on);
    void setFtpActive(bool on);
    void setTerminalActive(bool on);
    void setChatActive(bool on);
    void checkServiceStatus();
signals:
    void serviceReset(quint64 flag);
public slots:
    void onAboutToQuit();
    void onCheckServiceStatus();
private:
    Q_INVOKABLE void init();
private:
    QPointer<QKxFtpServer> m_ftpServer;
    QPointer<QKxCapServer> m_capServer;
    QPointer<QKxPtyServer> m_ptyServer;
    QPointer<QTimer> m_timer;
    int m_countLeft;
    quint64 m_serviceFlag;
};

#endif // QKXSERVICMANAGER_H
