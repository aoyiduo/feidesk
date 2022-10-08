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

#include "qkxptyclient.h"
#include "qkxzip.h"

#ifdef Q_OS_WIN
#include "qkxptyprocess_win.h"
#else
#include "qkxptyprocess_unix.h"
#endif

#include <QDebug>
#include <QCoreApplication>

QKxPtyClient::QKxPtyClient(QIODevice *local, QObject *parent)
    : QObject(parent)
    , m_local(local)
{
    m_stream.setDevice(local);
    QObject::connect(local, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    m_pty = new QKxPtyProcess(this);
    m_zip = new QKxZip(this);

    QObject::connect(m_pty, SIGNAL(finished(int)), this, SLOT(onProcessFinish()));
    QObject::connect(m_pty, SIGNAL(dataArrived(const char*,int)), this, SLOT(onPtyDataArrived(const char*,int)));
#if defined(Q_OS_WIN)
    qint8 platform = 1;
    QByteArray desc = "win";
#elif defined(Q_OS_MAC)
    qint8 platform = 2;
    QByteArray desc = "mac";
#elif defined(Q_OS_UNIX)
    qint8 platform = 3;
    QByteArray desc = "unix";
#elif defined(Q_OS_ANDROID)
    qint8 platform = 4;
    QByteArray desc = "andriod";
#endif
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << qint8(2) << platform << desc;
    m_stream << buf;
}

void QKxPtyClient::onReadyRead()
{
    QByteArray all = m_local->readAll();
    handlePacket(all);
}

void QKxPtyClient::onProcessFinish()
{

}

void QKxPtyClient::onPtyDataArrived(const char* pbuf, int cnt)
{
    QByteArray utf8 = QByteArray::fromRawData(pbuf, cnt);
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    if(utf8.length() > 1000) {
        QByteArray result;
        if(m_zip->encode(utf8, result) < 0) {
            return;
        }
        //qDebug() << "onPtyDataArrived" << utf8.length() << " compress" << result.length();
        ds << qint8(11) << result;
    }else{
        ds << qint8(1) << utf8;
    }
    m_stream << buf;
}

void QKxPtyClient::handlePacket(const QByteArray &buf)
{
    append(buf);
    tryTohandlePacket();
}


bool QKxPtyClient::tryTohandlePacket()
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
        if(!buf.isEmpty()) {
            m_stream << buf;
        }
    }
    return true;
}

bool QKxPtyClient::handleNextPacket(QDataStream &in, QDataStream &out)
{
    qint8 type;
    in >> type;

    if(type == 1) {
        if(m_pty) {
            QByteArray data;
            in >> data;
            m_pty->write(data.data(), data.length());
        }
    }else if(type == 2) {
        if(m_pty) {
            int cols, rows;
            in >> cols >> rows;
            qDebug() << "handleNextPacket 2" << cols << rows;
            m_pty->updateSize(cols, rows);
        }
    }else if(type == 3) {
        if(m_pty) {
            m_pty->sendCtrlC();
        }
    }
    return true;
}

QByteArray QKxPtyClient::enterToEnterLine(QByteArray buf)
{
    return buf;
}
