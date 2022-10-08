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

#include "qkxdesktopplaywindow.h"
#include "ui_qkxdesktopplaywindow.h"
#include "qkxdesktopplayform.h"
#include "qkxdesktoptoolform.h"
#include "qkxdesktopftpwidget.h"
#include "qkxnetsimpleclient.h"
#include "qkxdesktopplaystatusform.h"
#include "qkxdeskdef.h"


#include <QScrollArea>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QDebug>
#include <QScrollBar>
#include <QTimer>
#include <QInputDialog>
#include <QMessageBox>
#include <QIcon>

#define DURATION_DEFAULT        (150)

QKxDesktopPlayWindow::QKxDesktopPlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxDesktopPlayWindow)
    , m_isPasswordRight(true)
    , m_mode(0)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resource/skin/mstsc2.png"));
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WA_DeleteOnClose);

    m_deviceId = qgetenv("REMOTE_DEVICE_ID").toLongLong();
    m_friendlyName = qgetenv("LOCAL_FRIENDLY_NAME");
    m_password = QByteArray::fromBase64(qgetenv("REMOTE_DEVICE_TOKEN"));
    m_fromAccount = qgetenv("SERVICE_ACCOUNT").toInt() > 0;

    setWindowTitle(tr("Remote Desktop")+":"+QString::number(m_deviceId));

    m_area = new QScrollArea(this);
    m_player = new QKxDesktopPlayForm(m_area);

    m_area->setObjectName("playArea");
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setStyleSheet("QWidget#playArea{background-color: rgb(6, 6, 6);}");
    m_area->setContentsMargins(0, 0, 0, 0);
    m_area->setWidget(m_player);

    m_tool = new QKxDesktopToolForm(m_player, this);
    QObject::connect(m_tool, SIGNAL(enter()), this, SLOT(onToolEnter()));
    QObject::connect(m_tool, SIGNAL(leave()), this, SLOT(onToolLeave()));    
    m_tool->raise();
    QObject::connect(m_player, SIGNAL(screenCountChanged(int)), m_tool, SLOT(onScreenCountChanged(int)));
    QObject::connect(m_player, SIGNAL(messageSupport(int)), m_tool, SLOT(onMessageSupport(int)));
    QObject::connect(m_player, SIGNAL(snapshotArrived(qint64,int)), this, SLOT(onSnapshotArrived(qint64,int)));
    QObject::connect(m_tool, SIGNAL(qualityLevel(int)), m_player, SLOT(onSetQualityLevel(int)));
    QObject::connect(m_tool, SIGNAL(ftpClicked()), this, SLOT(onFtpClicked()));

    QSize sz = m_tool->sizeHint();
    m_tool->setGeometry(QRect((sz.width() - sz.width()) / 2, -1, sz.width(), sz.height()));
    QObject::connect(m_tool, SIGNAL(sizeChanged(QSize)), this, SLOT(onToolSizeChanged(QSize)), Qt::QueuedConnection);
    QObject::connect(m_tool, SIGNAL(nextScreenRatio()), m_player, SLOT(onNetScreenRatioRequest()));
    QObject::connect(m_tool, SIGNAL(nextScreenMode()), m_player, SLOT(onNextScreenModeRequest()));
    QObject::connect(m_tool, SIGNAL(blackScreen()), m_player, SLOT(onBlackScreen()));
    QObject::connect(m_tool, SIGNAL(lockScreen()), m_player, SLOT(onLockScreen()));

    QByteArray url = qgetenv("NETHOST_PROCESS_URL");
    m_net = new QKxNetSimpleClient(url, this);
    QObject::connect(m_net, SIGNAL(ipcConnectedArrived()), this, SLOT(onIpcConnectedArrived()));
    QObject::connect(m_net, SIGNAL(exitArrived()), this, SLOT(onServiceExitArrived()));
    QObject::connect(m_net, SIGNAL(broadcastMessageArrived(qint32,qint32,QVariant)), this, SLOT(onBroadcastMessageArrived(qint32,qint32,QVariant)));
    QObject::connect(m_net, SIGNAL(remoteStatus(qint8,qint32,qint32,QVariant,QVariant)), this, SLOT(onRemoteStatus(qint8,qint32,qint32,QVariant,QVariant)));
    QObject::connect(m_net, SIGNAL(remoteStatus(qint8,qint32,qint32,QVariant,QVariant)), m_player, SLOT(onRemoteStatus(qint8,qint32,qint32,QVariant,QVariant)));
    QObject::connect(m_net, SIGNAL(peerMessageArrived(qint32,QByteArray)), this, SLOT(onPeerMessageArrived(qint32,QByteArray)));

    m_status = new QKxDesktopPlayStatusForm(this);
    QObject::connect(m_status, SIGNAL(tryAgain()), this, SLOT(onTryAgainLater()));
    m_tool->raise();
    m_tool->activateWindow();
}

QKxDesktopPlayWindow::~QKxDesktopPlayWindow()
{
    delete ui;
}

void QKxDesktopPlayWindow::onSnapshotArrived(qint64 did, int type)
{
    QMap<QString, QVariant> data;
    data.insert("type", type);
    data.insert("deviceId", did);
    data.insert("why", "snapshotArrived");
    m_net->broadcastMessage(MESSAGE_TARGET_HISTORY_MODEL, 1, data);
}

void QKxDesktopPlayWindow::onTryAgainLater()
{
    if(!m_isPasswordRight) {
        QInputDialog input(this);
        input.setWindowFlags(input.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        input.setMinimumWidth(350);
        input.setWindowTitle(tr("Connection password"));
        input.setLabelText(tr("Please input connection password"));
        int err = input.exec();
        if(err == 0) {
            return;
        }
        QString password = input.textValue();
        if(password.isEmpty()) {
            QMessageBox::information(this, tr("information"), tr("Please input a connection password."));
            return;
        }
        m_password = password.toLatin1();
    }
    m_isPasswordRight = true;
    m_status->setMessage(tr("connecting..."));
    m_status->setTryButtonVisible(false);
    QTimer::singleShot(1000, this, SLOT(onIpcConnectedArrived()));
}

void QKxDesktopPlayWindow::closeEvent(QCloseEvent *e)
{
    emit closeArrived();
    QWidget::closeEvent(e);
}

void QKxDesktopPlayWindow::resizeEvent(QResizeEvent *e)
{
    QSize sz = e->size();
    QWidget::resizeEvent(e);
    updateToolbarPosition();
    m_status->setGeometry(0, 0, sz.width(), sz.height());
}

void QKxDesktopPlayWindow::updateToolbarPosition()
{
    QSize szWin = size();
    m_area->setGeometry(QRect(QPoint(0, 0), szWin));
    QRect rt = m_tool->geometry();
    int y = rt.top();
    if(m_ani != nullptr) {
        QPoint pt = m_ani->endValue().toPoint();
        pt.setX((szWin.width() - rt.width()) / 2);
        m_ani->setEndValue(pt);
    }else{
        m_tool->move((szWin.width() - rt.width()) / 2, y);
    }
}

void QKxDesktopPlayWindow::onToolEnter()
{
    QRect rt = m_tool->geometry();
    if(rt.top() >= 0) {
        return;
    }
    if(m_ani != nullptr) {
        m_ani->stop();
        m_ani->deleteLater();
    }
    QPropertyAnimation *animation = new QPropertyAnimation(m_tool, "pos");
    animation->setObjectName("Enter");
    QObject::connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(onDeleteLater()));
    m_ani = animation;
    QPoint pt = rt.topLeft();
    animation->setDuration(DURATION_DEFAULT);
    animation->setStartValue(pt);
    QSize sz = size();
    pt.setX((sz.width() - rt.width()) / 2);
    pt.setY(-1);
    animation->setEndValue(pt);
    animation->start();
}

void QKxDesktopPlayWindow::onToolLeave()
{
    QRect rt = m_tool->geometry();
    int y = -rt.height() + 5;
    if(rt.top() <= y) {
        return;
    }
    if(m_ani != nullptr) {
        m_ani->stop();
        m_ani->deleteLater();
    }
    QPropertyAnimation *animation = new QPropertyAnimation(m_tool, "pos");
    animation->setObjectName("Leave");
    QObject::connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(onDeleteLater()));
    m_ani = animation;
    QPoint pt = m_tool->geometry().topLeft();
    animation->setDuration(DURATION_DEFAULT);
    animation->setStartValue(pt);
    QSize sz = size();
    pt.setX((sz.width() - rt.width()) / 2);
    pt.setY(y);
    animation->setEndValue(pt);
    animation->start();
}

void QKxDesktopPlayWindow::onDeleteLater()
{
    QObject *obj = sender();
    qDebug() << "onDeleteLater" << obj->objectName();
}

void QKxDesktopPlayWindow::onToolSizeChanged(const QSize &sz)
{
    updateToolbarPosition();
}

void QKxDesktopPlayWindow::onConnectTimeout()
{
    if(m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
    }
    if(m_player->isConnected()) {
        return;
    }
    m_status->setMessage(tr("Timeout to connect."));
    m_status->setTryButtonVisible(true);
}

void QKxDesktopPlayWindow::onMessageReceived(const QString &msg)
{
    if(msg == "active") {
        if(isMinimized()) {
            showNormal();
        }
        raise();
        activateWindow();
    }
}

void QKxDesktopPlayWindow::onFtpClicked()
{
    if(m_ftp == nullptr) {
        m_ftp = new QKxDesktopFtpWidget(QString::number(m_deviceId), m_player, this);
        QObject::connect(m_status, SIGNAL(showArrived()), m_ftp, SLOT(close()));
        m_ftp->resize(800, 600);
        m_ftp->hide();
    }
    QSize szFtp = m_ftp->size();
    QSize sz = size();
    int x = (sz.width() - szFtp.width()) / 2;
    int y = (sz.height() - szFtp.height()) / 2;
    m_ftp->setGeometry(x, y, szFtp.width(), szFtp.height());
    m_ftp->show();
}

void QKxDesktopPlayWindow::onIpcConnectedArrived()
{
    if(m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
    }
    m_reconnected = true;
    m_timer = new QTimer();
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onConnectTimeout()));
    m_timer->start(1000 * 25);
    QVariantMap dm;
    dm.insert("password", m_password);
    dm.insert("friendlyName", m_friendlyName);
    dm.insert("mode", m_mode);
    m_net->openRemote(m_deviceId, 1, m_fromAccount ? 1 : 0, dm, 1);
}

void QKxDesktopPlayWindow::onNetStateArrived(int state, const QString &msg)
{

}

void QKxDesktopPlayWindow::onServiceExitArrived()
{
    QCoreApplication::quit();
}

void QKxDesktopPlayWindow::onBroadcastMessageArrived(qint32 target, qint32 event, const QVariant &v)
{

}

void QKxDesktopPlayWindow::onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant &v, const QVariant &data)
{
    qDebug() << "onRemoteStatus" << type << step << err << v.toString();
    if(step == 0) {
        // ERSTaskBegin
        m_status->setMessage(tr("start to build connection."));
        m_status->setVisible(true);
        m_status->setTryButtonVisible(false);
    }else if(step == 1) {
        // ERSTaskEnd
    }else if(step == 2) {
        // ERSPeerConnection
        if(err == -4) {
            m_status->setMessage(tr("the device is online."));
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }else if(err == ERROR_DEVICE_PASSWORD_WRONG) {
            m_status->setMessage(tr("password is wrong."));
            m_isPasswordRight = false;
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }else if(err == ERROR_SESSION_TO_SESSION_CONNECT_TIMEOUT) {
            qInfo() << "session to session: connect timeout." << tr("access closed.");
            m_status->setMessage(tr("Connection session timed out."));
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }else if(err == ERROR_PEER_TO_PEER_CONNECT_TIMEOUT) {
            QVariantMap dm = v.toMap();
            bool isTurn = dm.value("IsTurn").toBool();
            if(!isTurn) {
                // maybe punch hole between peer, so change to turn mode.
                m_mode = 1;
            }
            qInfo() << "peer to peer: connect timeout";
            m_status->setMessage(tr("Connection peer timed out."));
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }
    }else if(step == 3 && err == 0) {
        // ERSPortMapping
        m_status->setVisible(false);
        m_status->setTryButtonVisible(false);
        if(m_timer) {
            m_timer->stop();
            m_timer->deleteLater();
        }
    }else{
        QString msg = v.toString();
        if(!msg.isEmpty()) {
            m_status->setVisible(true);
            m_status->setMessage(tr(msg.toUtf8()));
        }
        m_status->setTryButtonVisible(err < 0);
    }
}

void QKxDesktopPlayWindow::onPeerMessageArrived(qint32 type, const QByteArray &msg)
{
    qDebug() << "onPeerMessageArrived" << type << msg;
    if(type == PEER_MESSAGE_MANUAL_CLOSE_PEER) {
        m_status->setSplashMessage(msg);
        m_status->setVisible(true);
    }
}
