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

#ifndef QKXPTYREQUESTPRIVATE_H
#define QKXPTYREQUESTPRIVATE_H

#include "qkxlengthbodypacket.h"

#include <QObject>
#include <QPointer>
#include <QDataStream>

class QTcpSocket;
class QKxZip;
class QKxPtyRequestPrivate : public QObject, public QKxLengthBodyPacket
{
    Q_OBJECT
public:
    explicit QKxPtyRequestPrivate(QObject *parent = nullptr);
    ~QKxPtyRequestPrivate();
    bool connectToServer(const QString& host, int port);
    void sendData(const QByteArray& buf);
    void updateSize(int cols, int rows);
    void sendCtrlC();
signals:
    void dataArrived(const QByteArray& buf);
    void errorArrived(const QByteArray& msg);
    void platformTypeArrived(int platform, const QByteArray& os);
    void connected();
    void disconnected();
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
private:
    void handlePacket(const QByteArray& buf);
    bool tryTohandlePacket();
    bool handleNextPacket(QDataStream& in, QDataStream& out);
private:
    QPointer<QTcpSocket> m_tcp;
    QDataStream m_stream;
    QPointer<QKxZip> m_zip;
};

#endif // QKXPTYREQUESTPRIVATE_H
