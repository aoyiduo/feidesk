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

#ifndef QKXPTYSERVER_H
#define QKXPTYSERVER_H
#include "qkxpty_share.h"

#include <QObject>
#include <QPointer>

class QKxPtyServerPrivate;
class KXPTY_EXPORT QKxPtyServer : public QObject
{
    Q_OBJECT
public:
    explicit QKxPtyServer(const QString& envName, QObject *parent = nullptr);
    virtual ~QKxPtyServer();
signals:
private:
    QPointer<QKxPtyServerPrivate> m_prv;
};

#endif // QKXPTYSERVER_H
