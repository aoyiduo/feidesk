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

#ifndef QKXPOWERNETSERVER_H
#define QKXPOWERNETSERVER_H

#include <QObject>

class QKxPowerNetServer : public QObject
{
    Q_OBJECT
public:
    explicit QKxPowerNetServer(QObject *parent = nullptr);
};

#endif // QKXPOWERNETSERVER_H
