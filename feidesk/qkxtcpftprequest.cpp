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

#include "qkxtcpftprequest.h"

#include <QTcpSocket>

QKxTcpFtpRequest::QKxTcpFtpRequest(const QString &serverUrl, QObject *parent)
    : QKxFtpRequest(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection, Q_ARG(QString, serverUrl));
}

void QKxTcpFtpRequest::writePacket(const QByteArray &data)
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << data;
    m_tcp->write(buf);
}

void QKxTcpFtpRequest::onReadyRead()
{
    QByteArray all = m_tcp->readAll();
    handlePacket(all);
}

void QKxTcpFtpRequest::init(const QString &server)
{
    m_tcp = new QTcpSocket(this);
    QObject::connect(m_tcp, SIGNAL(connected()), this, SIGNAL(connectedArrived()));
    QObject::connect(m_tcp, SIGNAL(disconnected()), this, SIGNAL(disconnectedArrived()));
    QObject::connect(m_tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(disconnectedArrived()));
    QObject::connect(m_tcp, SIGNAL(aboutToClose()), this, SIGNAL(disconnectedArrived()));
    QObject::connect(m_tcp, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    QStringList hps = server.split(':');
    QString host = hps.at(1);
    QString port = hps.at(2);
    if(host.startsWith("//")) {
        host = host.mid(2);
    }
    if(host.isEmpty() || host == "0.0.0.0" || host == "::") {
        host = "127.0.0.1";
    }
    m_tcp->connectToHost(host, port.toInt());
}
