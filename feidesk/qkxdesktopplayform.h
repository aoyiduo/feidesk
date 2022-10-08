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

#ifndef QKXDESKTOPPLAYFORM_H
#define QKXDESKTOPPLAYFORM_H

#include <QPointer>
#include "qkxvncwidget.h"

class QKxDesktopPlayForm : public QKxVncWidget
{
    Q_OBJECT

public:
    explicit QKxDesktopPlayForm(QWidget *parent = 0);
    ~QKxDesktopPlayForm();
signals:
    void snapshotArrived(qint64 did, int type);
private slots:
    void onSetQualityLevel(int lv);
    void onSnapshotPrepare();
    void onSnapshotTimeout();
    void onBlackScreen();
    void onLockScreen();
    void onNetScreenRatioRequest();
    void onNextScreenModeRequest();
    void onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant & v, const QVariant & data);

    //vnc
    void onQpsArrived(int qps);
    void onFinished();
    void onConnectionStart();
    void onConnectionFinished(bool ok);
    void onErrorArrived(const QByteArray& buf);
    void onPasswordResult(const QByteArray& passwd, bool ok);
    void onInputArrived(const QString& prompt, bool visible);
    void onPasswordInputResult(const QString& pass, bool isSave);
private:
    void reconnect();
protected:
    virtual void resizeEvent(QResizeEvent *ev);
private:
    QString m_host;
    quint16 m_port;
    bool m_black;
};

#endif // QKXDESKTOPPLAYFORM_H
