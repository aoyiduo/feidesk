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

#include "qkxdesktopftprequest.h"

#include "qkxvncwidget.h"

#include <QDebug>
#include <QDataStream>

QKxDesktopFtpRequest::QKxDesktopFtpRequest(QKxVncWidget *vnc, QObject *parent)
    : QKxFtpRequest(parent)
    , m_vnc(vnc)
{
    QObject::connect(vnc, SIGNAL(ftpArrived(QByteArray)), this, SLOT(onFtpResult(QByteArray)));
}

void QKxDesktopFtpRequest::writePacket(const QByteArray &data)
{
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << data;
    m_vnc->sendFtpPacket(buf);
}

void QKxDesktopFtpRequest::onFtpResult(const QByteArray &buf)
{
    handlePacket(buf);
}
