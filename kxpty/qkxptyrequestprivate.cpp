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

#include "qkxptyrequestprivate.h"
#include <QTcpSocket>

#include "qkxzip.h"

QKxPtyRequestPrivate::QKxPtyRequestPrivate(QObject *parent)
    : QObject(parent)
{
    m_zip = new QKxZip(this);
}

QKxPtyRequestPrivate::~QKxPtyRequestPrivate()
{

}

bool QKxPtyRequestPrivate::connectToServer(const QString &host, int port)
{
    if(m_tcp) {
        m_tcp->deleteLater();
    }
    m_tcp = new QTcpSocket(this);
    QObject::connect(m_tcp, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_tcp, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    QObject::connect(m_tcp, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    m_tcp->connectToHost(host, port);
    m_stream.setDevice(m_tcp);
    return true;
}

void QKxPtyRequestPrivate::sendData(const QByteArray &data)
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::ReadWrite);
    ds << qint8(1) << data;
    m_stream << buf;
}

void QKxPtyRequestPrivate::updateSize(int cols, int rows)
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::ReadWrite);
    ds << qint8(2) << cols << rows;
    m_stream << buf;
}

void QKxPtyRequestPrivate::sendCtrlC()
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::ReadWrite);
    ds << qint8(3);
    m_stream << buf;
}

void QKxPtyRequestPrivate::onConnected()
{
    emit connected();
}

void QKxPtyRequestPrivate::onDisconnected()
{
    emit disconnected();
}

void QKxPtyRequestPrivate::onReadyRead()
{
    QByteArray buf = m_tcp->readAll();
    handlePacket(buf);
}

void QKxPtyRequestPrivate::handlePacket(const QByteArray &buf)
{
    append(buf);
    tryTohandlePacket();
}


bool QKxPtyRequestPrivate::tryTohandlePacket()
{
    while(packetAvailable()) {
        QByteArray raw = nextPacket();
        if(raw.isEmpty()) {
            return true;
        }
        QDataStream in(raw);
        QByteArray buf;
        QDataStream out(&buf, QIODevice::ReadWrite);
        if(!handleNextPacket(in, out)) {
            return false;
        }
    }
    return true;
}

bool QKxPtyRequestPrivate::handleNextPacket(QDataStream &in, QDataStream &out)
{
    qint8 type;
    in >> type;
    if(type == 1) {
        QByteArray data;
        in >> data;
        emit dataArrived(data);
    }else if(type == 2) {
        qint8 platform;
        QByteArray os;
        in >> platform >> os;
        emit platformTypeArrived(platform, os);
    }else if(type == 11) {
        QByteArray data, result;
        in >> data;
        m_zip->decode(data, result);
        emit dataArrived(result);
    }
    return true;
}
