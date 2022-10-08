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

#ifndef QKXTERMWIDGETEX_H
#define QKXTERMWIDGETEX_H

#include "qkxtermwidget.h"

class QKxEchoInput;
class QKxPtyRequest;
class QMenu;

class QKxTermWidgetEx : public QKxTermWidget
{
    Q_OBJECT
public:
    explicit QKxTermWidgetEx(QWidget *parent = nullptr);
    void initDefault();
    void initCustom();
    void addToTermImpl();
private slots:
    void onPtyDataArrived(const QByteArray& buf);
    void onTermCtrlCArrived();
    void onTermSizeChanged(int lines, int columns);
    void onTermSendData(const QByteArray& buf);
    void onConnected();
    void onDisconnected();
    void onPlatformTypeArrived(int platform, const QByteArray& os);
    void onCopyToClipboard();
    void onPasteFromClipboard();
    void onShowFindBar();

    void onPeerConnectedArrived(const QString& addr);
protected:
    virtual void contextMenuEvent(QContextMenuEvent *ev);
private:
    Q_INVOKABLE void reconnect();
private:
    void resetProperty(QVariantMap data);
private:
    QPointer<QKxEchoInput> m_input;
    QPointer<QKxPtyRequest> m_pty;
    QString m_serverAddr;
};

#endif // QKXTERMWIDGETEX_H
