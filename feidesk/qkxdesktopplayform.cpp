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

#include "qkxdesktopplayform.h"
#include "qkxdeskdef.h"
#include "qkxdesktopplaystatusform.h"
#include "qkxwallpaper.h"
#include "qkxsettingex.h"
#include "qkxnet_share.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>

QKxDesktopPlayForm::QKxDesktopPlayForm(QWidget *parent)
    : QKxVncWidget(parent)
{
    QObject::connect(this, SIGNAL(qpsArrived(int)), this, SLOT(onQpsArrived(int)));
    QObject::connect(this, SIGNAL(finished()), this, SLOT(onFinished()));
    QObject::connect(this, SIGNAL(connectionStart()), this, SLOT(onConnectionStart()));
    QObject::connect(this, SIGNAL(connectionFinished(bool)), this, SLOT(onConnectionFinished(bool)));
    QObject::connect(this, SIGNAL(errorArrived(QByteArray)), this, SLOT(onErrorArrived(QByteArray)));
    QObject::connect(this, SIGNAL(passwordResult(QByteArray,bool)), this, SLOT(onPasswordResult(QByteArray,bool)));
    QObject::connect(this, SIGNAL(inputArrived(QString,bool)), this, SLOT(onInputArrived(QString,bool)));
    //QObject::connect(this, SIGNAL(screenCountChanged(int)), this, SLOT(onSnapshotPrepare()));

    setAttribute(Qt::WA_InputMethodEnabled, true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    bool show = QKxSettingEx::value("desktop/localMouse", false).toBool();
    setLocalCursorVisible(show);
}

QKxDesktopPlayForm::~QKxDesktopPlayForm()
{

}

void QKxDesktopPlayForm::onSetQualityLevel(int lv)
{
    setQualityLevel(lv);
}

void QKxDesktopPlayForm::onSnapshotPrepare()
{
    QTimer::singleShot(1000 * 8, this, SLOT(onSnapshotTimeout()));
}

void QKxDesktopPlayForm::onSnapshotTimeout()
{
    qDebug() << "onSnapshotTimeout";
    QImage img = capture();
    if(!img.isNull()) {
        QString path = QKxSettingEx::applicationDataPath() + "/history";
        QFileInfo fi(path);
        if(fi.exists()) {
            if(!fi.isDir()) {
                if(!fi.isFile()) {
                    return;
                }
                if(!QFile::remove(path)) {
                    return;
                }
                QDir d;
                d.mkpath(path);
            }
        }else{
            QDir d;
            d.mkpath(path);
        }
        QImage icon(320, 200, QImage::Format_RGB32);
        icon.fill(0);
        QPainter p(&icon);
        int hsrc = img.height();
        int hdst = 320 * img.height() / img.width();
        if(hdst > 200) {
            hdst = 200;
            hsrc = 200 * img.width() / 320;
        }
        int ydst = (200 - hdst) / 2;
        p.drawImage(QRect(0, ydst, 320, hdst), img, QRect(0, 0, img.width(), hsrc));
        QByteArray did = qgetenv("REMOTE_DEVICE_ID");
        path += QString("/desk%1.jpg").arg(QString(did));
        icon.save(path);

        emit snapshotArrived(did.toLongLong(), 1);
    }
}

void QKxDesktopPlayForm::onBlackScreen()
{
    setBlackScreen();
}

void QKxDesktopPlayForm::onLockScreen()
{
    setLockScreen();
}

void QKxDesktopPlayForm::onNetScreenRatioRequest()
{
    qDebug() << "onNetScreenRatioRequest";
    setNextRatio();
}

void QKxDesktopPlayForm::onNextScreenModeRequest()
{
    qDebug() << "onNextScreenModeRequest";
    setNextScreen();
}

void QKxDesktopPlayForm::onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant & v, const QVariant & data)
{
    qDebug() << "onRemoteStatus" << type << step << err << v.toString();
    if(step == 0) {
        // ERSTaskBegin
    }else if(step == 1) {
        // ERSTaskEnd
    }else if(step == 2) {
        // ERSPeerConnection
        if(err == -4) {

        }else if(err == ERROR_DEVICE_PASSWORD_WRONG) {

        }
    }else if(step == 3 && err == 0) {
        // ERSPortMapping
        QString addr = v.toString();
        if(addr.startsWith("tcp:")) {
            QStringList hp = addr.split(':');
            QString host = hp.at(1);
            QString port = hp.at(2);
            if(host.startsWith("//")) {
                host = host.mid(2);
            }
            m_host = host;
            m_port = port.toInt();
            reconnect();
        }
    }
}

void QKxDesktopPlayForm::onQpsArrived(int qps)
{
    qDebug() << "qps" << qps;
}

void QKxDesktopPlayForm::onFinished()
{

}

void QKxDesktopPlayForm::onConnectionStart()
{

}

void QKxDesktopPlayForm::onConnectionFinished(bool ok)
{

}

void QKxDesktopPlayForm::onErrorArrived(const QByteArray &buf)
{

}

void QKxDesktopPlayForm::onPasswordResult(const QByteArray &passwd, bool ok)
{

}

void QKxDesktopPlayForm::onInputArrived(const QString &prompt, bool visible)
{

}

void QKxDesktopPlayForm::onPasswordInputResult(const QString &pass, bool isSave)
{

}

void QKxDesktopPlayForm::reconnect()
{
    QKxVNC::EPixelFormat fmt = QKxVNC::H264_Normal;
    QKxVNC::EProtoVersion ver = QKxVNC::RFB_38;
    QVector<QKxVNC::EEncodingType> encs;
    encs << QKxVNC::OpenH264
         << QKxVNC::ZRLE3
         << QKxVNC::TRLE3
         << QKxVNC::ZRLE2
         << QKxVNC::TRLE2
         << QKxVNC::RRE
         << QKxVNC::CopyRect
         << QKxVNC::DesktopSizePseudoEncoding;
    start(m_host, m_port, VNC_SERVER_PASSWORD, fmt, ver, encs);
}

void QKxDesktopPlayForm::resizeEvent(QResizeEvent *ev)
{
    QImage img = capture();
    if(!img.isNull()) {

    }
}
