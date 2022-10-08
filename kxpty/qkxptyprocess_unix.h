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
#include <QPointer>

class KPtyDevice;
class QKxPtyProcess : public QProcess
{
    Q_OBJECT
public:
    explicit QKxPtyProcess(QObject *parent = nullptr);
    virtual ~QKxPtyProcess();

    int updateSize(int cols, int rows);
    void sendCtrlC();
    void write(const char *pbuf, int cnt);
signals:
    void dataArrived(const char* pbuf, int cnt);
protected:
    virtual void setupChildProcess();
private slots:
    void onReadyRead();
private:
    Q_INVOKABLE void init();
    void setUtf8Mode(bool on);
    void setFlowControlEnabled(bool enable);
private:
    bool _utf8;
    bool _xonXoff;
    QPointer<KPtyDevice> m_pty;
};

#endif // QKXPTYPROCESS_H
