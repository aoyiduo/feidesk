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

#include "qkxptyserverprivate.h"
#include "qkxptyclient.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QThread>
#include <QLibraryInfo>

QKxPtyServerPrivate::QKxPtyServerPrivate(const QString &envName)
    : QThread(nullptr)
    , m_envName(envName)
    , m_bClose(false)
{
    m_worker = new QThread(nullptr);
    moveToThread(m_worker);
    m_worker->start();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

QKxPtyServerPrivate::~QKxPtyServerPrivate()
{
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
    m_worker->wait();
    delete m_worker;
}

void QKxPtyServerPrivate::onNewConnection()
{
    QTcpServer *server = qobject_cast<QTcpServer*>(sender());
    while(server->hasPendingConnections()) {
        QTcpSocket *local = server->nextPendingConnection();
        QKxPtyClient *client = new QKxPtyClient(local, this);
        QObject::connect(local, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        m_clients.insert(local, client);
    }
}

void QKxPtyServerPrivate::onDisconnected()
{
    if(m_bClose) {
        qDebug() << "onLocalDisconnected";
        return;
    }
    QIODevice *dev = qobject_cast<QIODevice*>(sender());
    QString msg = dev->errorString();
    QKxPtyClient *client = m_clients.take(dev);
    if(client != nullptr) {
        client->deleteLater();
    }
    dev->deleteLater();
}

void QKxPtyServerPrivate::init()
{
#ifdef QT_DEBUG
    QString addr = "0.0.0.0";
    int port = 3721;
#else
    QString addr = "127.0.0.1";
    int port = 0;
#endif
    m_server = new QTcpServer(this);
    QObject::connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    if(!m_server->listen(QHostAddress(addr), port)) {
        qFatal("it's never come here");
    }
    QString _addr = QString("tcp://%1:%2").arg(addr).arg(m_server->serverPort());
    QByteArray addr2 = _addr.toLatin1();
    qputenv(m_envName.toLatin1(), addr2);
    qDebug() << "Pty Listen Address" << addr2;
}

void QKxPtyServerPrivate::close()
{
    qputenv(m_envName.toLatin1(), "");
    m_bClose = true;
    for(QMap<QPointer<QIODevice>, QPointer<QKxPtyClient> >::Iterator it = m_clients.begin(); it != m_clients.end(); it++) {
        QIODevice *k = it.key();
        QKxPtyClient *v = it.value();
        delete k;
        delete v;
    }
    m_clients.clear();
    if(m_server) {
        m_server->close();
        delete m_server;
    }
    m_worker->quit();
}
