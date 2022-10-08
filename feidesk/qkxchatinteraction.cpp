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

#include "qkxchatinteraction.h"
#include "qkxnetsimpleclient.h"

#include <QDataStream>
#include <QDateTime>
#include <QTimer>

#include <algorithm>

#define HEARTBEAT_INTERVAL   (1500)

#define CHAT_PING           (1)
#define CHAT_PONG           (2)
#define CHAT_MESSAGE        (3)
#define CHAT_MESSAGE_ACK    (4)

QKxChatInteraction::QKxChatInteraction(qint64 did, QKxNetSimpleClient *net, QObject *parent)
    : QObject(parent)
    , m_net(net)
{
    m_timeAckLast = 0;
    m_state = EUndefined;
    m_idLast = 0;
    m_deviceIdLocal = net->deviceID();
    m_deviceIdRemote = did;
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onHeartBeatArrived()));
    m_timer->start(HEARTBEAT_INTERVAL);
    onHeartBeatArrived();

    QObject::connect(m_net, SIGNAL(peerMessageArrived(qint32,QByteArray)), this, SLOT(onMessageArrived(qint32,QByteArray)));
}

QKxChatInteraction::~QKxChatInteraction()
{

}

void QKxChatInteraction::sendMessage(const QString &msg)
{
    QByteArray utf8 = msg.toUtf8();
    qint64 id = QDateTime::currentSecsSinceEpoch() * 1000 + (++m_idLast);
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << qint8(CHAT_MESSAGE) << id << utf8;
    sendMessage(buf);
    ChatMsg cm;
    cm.id = id;
    cm.idFrom = 0;
    cm.tmAck = 0;
    cm.didFrom = m_deviceIdLocal;
    cm.didTo = m_deviceIdRemote;
    cm.msg = utf8;
    m_history.append(cm);
}

void QKxChatInteraction::onMessageArrived(qint32 type, const QByteArray &_msg)
{
    if(type == PEER_MESSAGE_CHAT) {
        QDataStream ds(_msg);
        qint8 type;
        ds >> type;

        if(type == CHAT_MESSAGE) {
            qint64 idFrom;
            QByteArray utf8;
            ds >> idFrom >> utf8;

            sendMessageAck(idFrom);

            qint64 id = QDateTime::currentSecsSinceEpoch() * 1000 + (++m_idLast);
            ChatMsg cm;
            cm.tmAck = -1;
            cm.id = id;
            cm.idFrom = idFrom;
            cm.didFrom = m_deviceIdRemote;
            cm.didTo = m_deviceIdLocal;
            emit messageArrived(QString::fromUtf8(utf8));
        }else if(type == CHAT_PING) {
            sendPong();
        }else if(type == CHAT_PONG) {
            //Pong response from remote side.
            m_timeAckLast = QDateTime::currentMSecsSinceEpoch();
        }else if(type == CHAT_MESSAGE_ACK) {
            //Ack response from remote side.
            qint64 id;
            ds >> id;
            ChatMsg cm;
            cm.id = id;
            auto it = std::lower_bound(m_history.begin(), m_history.end(), cm);
            if(!(it == m_history.end() ||  cm < *it)) {
                ChatMsg& hit = *it;
                hit.tmAck = QDateTime::currentSecsSinceEpoch();
            }

            m_timeAckLast = QDateTime::currentMSecsSinceEpoch();
        }
    }
}

void QKxChatInteraction::onHeartBeatArrived()
{
    int ms = m_timer->interval();
    sendPing();
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if((now - m_timeAckLast) < (ms * 3)) {
        if(m_state != EActived) {
            m_state = EActived;
            emit stateChanged(m_state);
        }
    }else{
        if(m_state != EDeactived) {
            m_state = EDeactived;
            emit stateChanged(m_state);
        }
    }
}

void QKxChatInteraction::sendMessage(const QByteArray &msg)
{
    m_net->sendPeerMessage(PEER_MESSAGE_CHAT, msg);
}

void QKxChatInteraction::sendMessageAck(qint64 id)
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << qint8(CHAT_MESSAGE_ACK) << id;
    sendMessage(buf);
}

void QKxChatInteraction::sendPing()
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << qint8(CHAT_PING);
    sendMessage(buf);
}

void QKxChatInteraction::sendPong()
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << qint8(CHAT_PONG);
    sendMessage(buf);
}
