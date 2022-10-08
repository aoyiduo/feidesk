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

#ifndef QKXPTYREQUEST_H
#define QKXPTYREQUEST_H

#include "qkxpty_share.h"

#include <QObject>
#include <QPointer>


class QKxPtyRequestPrivate;
class KXPTY_EXPORT QKxPtyRequest : public QObject
{
    Q_OBJECT
public:
    explicit QKxPtyRequest(QObject *parent = nullptr);
    virtual ~QKxPtyRequest();
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
private:
    QPointer<QKxPtyRequestPrivate> m_prv;
};

#endif // QKXPTYREQUEST_H
