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

#ifndef QKXDESKDEF_H
#define QKXDESKDEF_H

#define SERVICE_NAME                ("FeiDeskService")
#define VNC_SERVER_PASSWORD         ("20220405") // 2022.04.05 init this password.

#define MY_DEVICE_MANAGE_HOST       ("my.aoyiduo.com")
#define MY_DEVICE_MANAGE_PORT       (80)   // fix port
#define MY_DEVICE_LOGIN_PORT        (5735)  // default port, can change by server configure.

#define MESSAGE_TARGET_BASE_ID              (0x10)
#define MESSAGE_TARGET_HISTORY_MODEL        (MESSAGE_TARGET_BASE_ID+1)


#endif // QKXDESKDEF_H
