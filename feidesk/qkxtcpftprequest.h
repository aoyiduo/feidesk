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

#ifndef QKXTCPFTPREQUEST_H
#define QKXTCPFTPREQUEST_H

#include <QPointer>

#include "qkxftprequest.h"
class QTcpSocket;
class QKxTcpFtpRequest : public QKxFtpRequest
{
    Q_OBJECT
public:
    explicit QKxTcpFtpRequest(const QString& serverUrl, QObject *parent = nullptr);
protected:
    virtual void writePacket(const QByteArray& data);
protected slots:
   void onReadyRead();
private:
   Q_INVOKABLE void init(const QString& serverUrl);
private:
   QPointer<QTcpSocket> m_tcp;
};

#endif // QKXTCPFTPREQUEST_H
