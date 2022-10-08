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

#ifndef QKXPTYSERVERPRIVATE_H
#define QKXPTYSERVERPRIVATE_H

#include <QPointer>
#include <QThread>
#include <QMap>

class QKxPtyClient;
class QTcpServer;
class QIODevice;
class QKxPtyServerPrivate : public QThread
{
    Q_OBJECT
public:
    explicit QKxPtyServerPrivate(const QString& envName);
    virtual ~QKxPtyServerPrivate();
private slots:
    void onNewConnection();
    void onDisconnected();
private:
    Q_INVOKABLE void init();
    Q_INVOKABLE void close();
private:
    QString m_envName;
    QPointer<QThread> m_worker;
    QPointer<QTcpServer> m_server;
    QMap<QPointer<QIODevice>, QPointer<QKxPtyClient> > m_clients;
    bool m_bClose;
};

#endif // QKXPTYSERVERPRIVATE_H
