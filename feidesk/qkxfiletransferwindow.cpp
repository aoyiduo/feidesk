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

#include "qkxfiletransferwindow.h"
#include "qkxnetsimpleclient.h"
#include "qkxtcpftprequest.h"
#include "qkxfiletransferstatusform.h"
#include "qkxftptransferwidget.h"
#include "qkxpowerapplication.h"
#include "qkxnet_share.h"
#include "qkxsettingex.h"

#include <QDebug>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QHeaderView>
#include <QListView>
#include <QFileSystemModel>
#include <QAction>
#include <QInputDialog>
#include <QFileIconProvider>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QResizeEvent>
#include <QClipboard>


QKxFileTransferWindow::QKxFileTransferWindow(QWidget *parent)
    : QWidget(parent)
    , m_mode(0)
{
    setContentsMargins(0, 0, 0, 0);
    setObjectName("fileTransfer");
    setAttribute(Qt::WA_DeleteOnClose);

    m_deviceId = qgetenv("REMOTE_DEVICE_ID").toLongLong();
    m_friendlyName = qgetenv("LOCAL_FRIENDLY_NAME");
    m_password = QByteArray::fromBase64(qgetenv("REMOTE_DEVICE_TOKEN"));
    m_fromAccount = qgetenv("SERVICE_ACCOUNT").toInt() > 0;
    m_reconnected = false;
    m_isPasswordRight = true;

    setWindowTitle(tr("File Transfer")+":"+QString::number(m_deviceId));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    m_pathTask = QKxSettingEx::ensurePath("task");
    m_transfer = new QKxFtpTransferWidget(m_pathTask, QString::number(m_deviceId), this);

    layout->addWidget(m_transfer);
    setLayout(layout);

    QByteArray url = qgetenv("NETHOST_PROCESS_URL");
    m_net = new QKxNetSimpleClient(url, this);
    QObject::connect(m_net, SIGNAL(ipcConnectedArrived()), this, SLOT(onIpcConnectedArrived()));
    QObject::connect(m_net, SIGNAL(exitArrived()), this, SLOT(onServiceExitArrived()));
    QObject::connect(m_net, SIGNAL(broadcastMessageArrived(qint32,qint32,QVariant)), this, SLOT(onBroadcastMessageArrived(qint32,qint32,QVariant)));
    QObject::connect(m_net, SIGNAL(peerMessageArrived(qint32,QByteArray)), this, SLOT(onPeerMessageArrived(qint32,QByteArray)));
    QObject::connect(m_net, SIGNAL(remoteStatus(qint8,qint32,qint32,QVariant,QVariant)), this, SLOT(onRemoteStatus(qint8,qint32,qint32,QVariant,QVariant)));

    m_status = new QKxFileTransferStatusForm(this);
    m_status->setVisible(true);
    QObject::connect(m_status, SIGNAL(tryAgain()), this, SLOT(onTryAgainLater()));
}

QKxFileTransferWindow::~QKxFileTransferWindow()
{
    if(m_transfer) {
        m_transfer->deleteLater();
    }
    if(m_ftpRequest) {
        m_ftpRequest->deleteLater();
    }
    if(m_net) {
        m_net->deleteLater();
    }
}

void QKxFileTransferWindow::onConnectTimeout()
{
    qDebug() << "onConnectTimeout";
    if(m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
    }
    if(m_ftpRequest != nullptr) {
        return;
    }
    m_status->setMessage(tr("Timeout to connect."));
    m_status->setTryButtonVisible(true);
}

void QKxFileTransferWindow::onTryAgainLater()
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


void QKxFileTransferWindow::onIpcConnectedArrived()
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
    m_net->openRemote(m_deviceId, 2, m_fromAccount ? 1 : 0, dm, 2);
}

void QKxFileTransferWindow::onNetStateArrived(int state, const QString &msg)
{

}

void QKxFileTransferWindow::onServiceExitArrived()
{
    QCoreApplication::quit();
}

void QKxFileTransferWindow::onBroadcastMessageArrived(qint32 target, qint32 event, const QVariant &v)
{

}

void QKxFileTransferWindow::onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant &v, const QVariant &data)
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
            m_status->setMessage(tr("The device is not online."));
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
            qInfo() << "session to session: connect timeout.";
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
            qInfo() << "peer to peer: connect timeout" << tr("access closed.");
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
        QString addr = v.toString();
        if(m_ftpRequest == nullptr || m_reconnected) {
            m_serverUrl = addr;
            reconnect();
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

void QKxFileTransferWindow::onPeerMessageArrived(qint32 type, const QByteArray &msg)
{
    qDebug() << "onPeerMessageArrived" << type << msg;
    if(type == PEER_MESSAGE_MANUAL_CLOSE_PEER){
        m_status->setVisible(true);
        m_status->setSplashMessage(tr("the connection has been close by manual"));
    }
}

void QKxFileTransferWindow::onMessageReceived(const QString &msg)
{
    qDebug() << "onMessageReceived" << msg;
    if(msg == "active") {
        if(isMinimized()) {
            showNormal();
        }
        raise();
        activateWindow();
    }
}

void QKxFileTransferWindow::onConnectedArrived()
{
    if(m_ftpRequest) {
        m_ftpRequest->entryInfoList("");
    }
}

void QKxFileTransferWindow::onDisconnectedArrived()
{
    m_status->setMessage(tr("Network interruption"));
    m_status->setTryButtonVisible(true);
    m_status->setVisible(true);
}

void QKxFileTransferWindow::resizeEvent(QResizeEvent *ev)
{
    QSize sz = ev->size();
    QWidget::resizeEvent(ev);
    m_status->setGeometry(0, 0, sz.width(), sz.height());
}

void QKxFileTransferWindow::reconnect()
{
    if(m_ftpRequest) {
        m_ftpRequest->deleteLater();
    }
    m_reconnected = false;
    m_ftpRequest = new QKxTcpFtpRequest(m_serverUrl, this);
    m_transfer->setRequest(m_ftpRequest);
    QObject::connect(m_ftpRequest, SIGNAL(connectedArrived()), this, SLOT(onConnectedArrived()));
    QObject::connect(m_ftpRequest, SIGNAL(disconnectedArrived()), this, SLOT(onDisconnectedArrived()));
}

bool QKxFileTransferWindow::listFiles(const QString &path, QStringList& all, int maxCount)
{
    QFileInfo fi(path);
    if(fi.isDir()) {
        QDir d(fi.absoluteFilePath());
        QFileInfoList fil = d.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < fil.length(); i++) {
            QFileInfo fi = fil.at(i);
            if(!listFiles(fi.absoluteFilePath(), all, maxCount)) {
                return false;
            }
        }
    }else{
        all.append(fi.absoluteFilePath());
    }
    return all.length() < maxCount;
}

