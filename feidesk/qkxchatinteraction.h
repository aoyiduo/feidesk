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

#ifndef QKXCHATINTERACTION_H
#define QKXCHATINTERACTION_H

#include <QObject>
#include <QPointer>

class QTimer;
class QKxNetSimpleClient;
class QKxChatInteraction : public QObject
{
    Q_OBJECT
public:
    enum EState {
        EUndefined,
        EActived,
        EDeactived
    };
    Q_ENUM(EState)
public:
    explicit QKxChatInteraction(qint64 did, QKxNetSimpleClient *net, QObject *parent = nullptr);
    ~QKxChatInteraction();
    void sendMessage(const QString& msg);
signals:
    void messageArrived(const QString& msg);
    // state:
    // 0: ok; 1:disconnect.
    void stateChanged(QKxChatInteraction::EState state);
private slots:
    void onMessageArrived(qint32 type, const QByteArray &msg);
    void onHeartBeatArrived();
private:
    void sendMessage(const QByteArray& msg);
    void sendMessageAck(qint64 id);
    void sendPing();
    void sendPong();
private:
    QPointer<QKxNetSimpleClient> m_net;
    QPointer<QTimer> m_timer;
    EState m_state;
    qint64 m_timeAckLast;
    quint8 m_idLast;
    qint64 m_deviceIdLocal, m_deviceIdRemote;
    struct ChatMsg {
        qint64 id;
        qint64 idFrom;
        qint32 tmAck;
        qint64 didFrom;
        qint64 didTo;
        QByteArray msg;

        bool operator<(const ChatMsg& o) const {
            return id < o.id;
        }
    };

    QList<ChatMsg> m_history;
};

#endif // QKXCHATINTERACTION_H
