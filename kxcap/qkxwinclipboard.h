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

#ifndef QKXWINCLIPBOARD_H
#define QKXWINCLIPBOARD_H

#include "qkxclipboard.h"
#include <QPointer>


class QKxWinClipboard : public QKxClipboard
{
public:
    explicit QKxWinClipboard(QObject *parent = nullptr);
    virtual ~QKxWinClipboard();
private:
    virtual QString text() const;
};

#endif // QKXWINCLIPBOARD_H
