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

#ifndef QKXPTYCLIENT_H
#define QKXPTYCLIENT_H

#include "qkxlengthbodypacket.h"

#include <QObject>
#include <QPointer>
#include <QDataStream>
#include <QCryptographicHash>

class QKxZip;
class QIODevice;
class QKxPtyProcess;
class QKxPtyClient : public QObject,
        public QKxLengthBodyPacket
{
    Q_OBJECT
public:
    explicit QKxPtyClient(QIODevice *local, QObject *parent = nullptr);

private slots:
    void onReadyRead();
    void onProcessFinish();
    void onPtyDataArrived(const char* pbuf, int cnt);
private:
    void handlePacket(const QByteArray& buf);
    bool tryTohandlePacket();
    bool handleNextPacket(QDataStream& in, QDataStream& out);
    QByteArray enterToEnterLine(QByteArray buf);
private:
    QPointer<QIODevice> m_local;
    QDataStream m_stream;
    QPointer<QKxPtyProcess> m_pty;
    QPointer<QKxZip> m_zip;
};

#endif // QKXPTYCLIENT_H
