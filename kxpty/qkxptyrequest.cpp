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

#include "qkxptyrequest.h"
#include "qkxptyrequestprivate.h"

QKxPtyRequest::QKxPtyRequest(QObject *parent)
    : QObject(parent)
{
    m_prv = new QKxPtyRequestPrivate(this);
    QObject::connect(m_prv, SIGNAL(dataArrived(QByteArray)), this, SIGNAL(dataArrived(QByteArray)));
    QObject::connect(m_prv, SIGNAL(errorArrived(QByteArray)), this, SIGNAL(errorArrived(QByteArray)));
    QObject::connect(m_prv, SIGNAL(connected()), this, SIGNAL(connected()));
    QObject::connect(m_prv, SIGNAL(platformTypeArrived(int,QByteArray)), this, SIGNAL(platformTypeArrived(int,QByteArray)));
}

QKxPtyRequest::~QKxPtyRequest()
{
    delete m_prv;
}

bool QKxPtyRequest::connectToServer(const QString &host, int port)
{
    return m_prv->connectToServer(host, port);
}

void QKxPtyRequest::sendData(const QByteArray &buf)
{
    m_prv->sendData(buf);
}

void QKxPtyRequest::updateSize(int cols, int rows)
{
    m_prv->updateSize(cols, rows);
}

void QKxPtyRequest::sendCtrlC()
{
    m_prv->sendCtrlC();
}
