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

#ifndef QKXTERM_SHARE_H
#define QKXTERM_SHARE_H

#ifdef QTERM_LIBRARY
    #define QTERM_EXPORT Q_DECL_EXPORT
#else
    #define QTERM_EXPORT Q_DECL_IMPORT
#endif

#endif // QKXTERM_SHARE_H
