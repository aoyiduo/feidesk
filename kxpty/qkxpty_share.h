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

#ifndef QKXPTY_SHARE_H
#define QKXPTY_SHARE_H

#if defined(KXPTY_LIBRARY)
#  define KXPTY_EXPORT Q_DECL_EXPORT
#else
#  define KXPTY_EXPORT Q_DECL_IMPORT
#endif

#endif // QKXPTY_SHARE_H
