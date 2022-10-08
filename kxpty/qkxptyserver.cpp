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

#include "qkxptyserver.h"
#include "qkxptyserverprivate.h"

QKxPtyServer::QKxPtyServer(const QString &envName, QObject *parent)
    : QObject(parent)
{
    m_prv = new QKxPtyServerPrivate(envName);
}

QKxPtyServer::~QKxPtyServer()
{
    delete m_prv;
}
