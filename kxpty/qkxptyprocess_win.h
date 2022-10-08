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

#ifndef QKXPTYPROCESS_H
#define QKXPTYPROCESS_H

#include <QProcess>

class QKxPtyProcess : public QProcess
{
    Q_OBJECT
public:
    explicit QKxPtyProcess(QObject *parent = nullptr);
    virtual ~QKxPtyProcess();

    int updateSize(int cols, int rows);
    void sendCtrlC();
signals:
    void dataArrived(const char*,int);
private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
private:
    Q_INVOKABLE void init();
private:
    bool m_bAttach;
};

#endif // QKXPTYPROCESS_H
