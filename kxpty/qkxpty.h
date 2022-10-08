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

#ifndef QKXPTY_H
#define QKXPTY_H

#include "qkxpty_share.h"

#include <QObject>

class KXPTY_EXPORT QKxPty : public QObject
{
    Q_OBJECT
public:
    explicit QKxPty(QObject *parent = nullptr);

signals:

public slots:
};

#endif // QKXPTY_H
