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

#include "qkxchatwindow.h"
#include "ui_qkxchatwindow.h"
#include "qkxnetsimpleclient.h"
#include "qkxchatstatusform.h"
#include "qkxchatinteraction.h"
#include "richedit/qkxtextbrowser.h"

#include <QIcon>
#include <QDebug>
#include <QTimer>
#include <QInputDialog>
#include <QResizeEvent>
#include <QMessageBox>
#include <QDateTime>

QKxChatWindow::QKxChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QKxChatWindow)
{
    ui->setupUi(this);

    m_history = new QKxTextBrowser(this);
    ui->vbox->addWidget(m_history);
    m_history->setReadOnly(true);
    m_history->show();
    m_history->setObjectName("chatHistory");


    setWindowIcon(QIcon(":/resource/skin/chat.png"));
    setContentsMargins(0, 0, 0, 0);
    setObjectName("chat");
    setAttribute(Qt::WA_DeleteOnClose);

    m_runAsServer = qgetenv("RUN_AS_SERVER") == "1";
    m_deviceId = qgetenv("REMOTE_DEVICE_ID").toLongLong();
    m_friendlyName = qgetenv("LOCAL_FRIENDLY_NAME");
    m_password = QByteArray::fromBase64(qgetenv("REMOTE_DEVICE_TOKEN"));
    m_fromAccount = qgetenv("SERVICE_ACCOUNT").toInt() > 0;
    m_reconnected = false;
    m_isPasswordRight = true;

    setWindowTitle(tr("Chat")+":"+QString::number(m_deviceId));

    QObject::connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(onButtonSendClicked()));
    QObject::connect(ui->input, SIGNAL(returnPressed()), this, SLOT(onButtonSendClicked()));


    QByteArray url = qgetenv("NETHOST_PROCESS_URL");
    m_net = new QKxNetSimpleClient(url, this);
    m_net->setAcceptDeviceMessage(m_deviceId);
    QObject::connect(m_net, SIGNAL(ipcConnectedArrived()), this, SLOT(onIpcConnectedArrived()));
    QObject::connect(m_net, SIGNAL(exitArrived()), this, SLOT(onServiceExitArrived()));
    QObject::connect(m_net, SIGNAL(broadcastMessageArrived(qint32,qint32,QVariant)), this, SLOT(onBroadcastMessageArrived(qint32,qint32,QVariant)));
    QObject::connect(m_net, SIGNAL(peerMessageArrived(qint32,QByteArray)), this, SLOT(onPeerMessageArrived(qint32,QByteArray)));
    if(!m_runAsServer) {
        QObject::connect(m_net, SIGNAL(remoteStatus(qint8,qint32,qint32,QVariant,QVariant)), this, SLOT(onRemoteStatus(qint8,qint32,qint32,QVariant,QVariant)));
    }

    m_status = new QKxChatStatusForm(this);
    m_status->setVisible(true);
    if(m_runAsServer){
        m_status->setFixMessage(tr("wait for connection"));
    }
    QObject::connect(m_status, SIGNAL(tryAgain()), this, SLOT(onTryAgainLater()));

    QObject::connect(this, SIGNAL(peerConnectedArrived(QString)), this, SLOT(onPeerConnectedArrived(QString)));

    m_chat = new QKxChatInteraction(m_deviceId, m_net, this);
    QObject::connect(m_chat, SIGNAL(messageArrived(QString)), this, SLOT(onChatMessageArrived(QString)));
    QObject::connect(m_chat, SIGNAL(stateChanged(QKxChatInteraction::EState)), this, SLOT(onChatStateChanged(QKxChatInteraction::EState)));
}

QKxChatWindow::~QKxChatWindow()
{
    delete ui;
}

void QKxChatWindow::onPeerConnectedArrived(const QString& addr)
{
}

void QKxChatWindow::onConnectTimeout()
{
    qDebug() << "onConnectTimeout";
    if(m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
    }

    m_status->setMessage(tr("Timeout to connect."));
    m_status->setTryButtonVisible(true && !m_runAsServer);
}

void QKxChatWindow::onTryAgainLater()
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


void QKxChatWindow::onIpcConnectedArrived()
{
    if(m_runAsServer) {
        m_status->setVisible(false);
        return;
    }
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
    m_net->openRemote(m_deviceId, 3, m_fromAccount ? 1 : 0, dm, 2);
}

void QKxChatWindow::onNetStateArrived(int state, const QString &msg)
{

}

void QKxChatWindow::onServiceExitArrived()
{
    QCoreApplication::quit();
}

void QKxChatWindow::onBroadcastMessageArrived(qint32 target, qint32 event, const QVariant &v)
{

}

void QKxChatWindow::onRemoteStatus(qint8 type, qint32 step, qint32 err, const QVariant &v, const QVariant &data)
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
            m_status->setTryButtonVisible(true && !m_runAsServer);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }else if(err == ERROR_DEVICE_PASSWORD_WRONG) {
            m_status->setMessage(tr("password is wrong."));
            m_isPasswordRight = false;
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true && !m_runAsServer);
            if(m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
            }
        }else if(err == ERROR_SESSION_TO_SESSION_CONNECT_TIMEOUT) {
            qInfo() << "session to session: connect timeout.";
            m_status->setMessage(tr("Connection session timed out."));
            m_status->setVisible(true);
            m_status->setTryButtonVisible(true && !m_runAsServer);
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
            m_status->setTryButtonVisible(true && !m_runAsServer);
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
        emit peerConnectedArrived(addr);
    }else{
        QString msg = v.toString();
        if(!msg.isEmpty()) {
            m_status->setVisible(true);
            m_status->setMessage(tr(msg.toUtf8()));
        }
        m_status->setTryButtonVisible(err < 0 && !m_runAsServer);
    }
}

void QKxChatWindow::onPeerMessageArrived(qint32 type, const QByteArray &msg)
{
    qDebug() << "onPeerMessageArrived" << type << msg;
    if(type == PEER_MESSAGE_MANUAL_CLOSE_PEER){
        m_status->setVisible(true);
        m_status->setSplashMessage(tr("the connection has been close by manual"));
    }
}

void QKxChatWindow::onMessageReceived(const QString &msg)
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

void QKxChatWindow::onChatMessageArrived(const QString &msg)
{
    QKxTextCursor tc(m_history->textCursor());
    QString name = tr("Remote User");
    QTextCharFormat fmt = tc.charFormat();
    fmt.setFontWeight(QFont::Bold);
    fmt.setForeground(QColor("black"));
    tc.movePosition(QTextCursor::End);
    tc.insertBlock();
    tc.insertText(name+":", QColor("red"));
    tc.insertBlock();
    tc.insertText(msg, fmt);
}

void QKxChatWindow::onChatStateChanged(QKxChatInteraction::EState state)
{
    if(state == QKxChatInteraction::EActived) {
        m_status->setVisible(false);
    }else{
        m_status->setVisible(true);
    }
}

void QKxChatWindow::onConnectedArrived()
{
}

void QKxChatWindow::onDisconnectedArrived()
{
    m_status->setMessage(tr("Network interruption"));
    m_status->setTryButtonVisible(true && !m_runAsServer);
    m_status->setVisible(true);
}

void QKxChatWindow::onButtonSendClicked()
{
    QString txt = ui->input->text();
    if(txt.isEmpty()) {
        return;
    }
    ui->input->clear();
    QKxTextCursor tc(m_history->textCursor());
    QString name = tr("Me");
    tc.movePosition(QTextCursor::End);
    tc.insertBlock();
    tc.insertText(name+":", QColor("blue"));
    tc.insertBlock();
    tc.insertText(txt, QColor("black"));

    m_chat->sendMessage(txt);
}

void QKxChatWindow::resizeEvent(QResizeEvent *ev)
{
    QSize sz = ev->size();
    QWidget::resizeEvent(ev);
    m_status->setGeometry(0, 0, sz.width(), sz.height());
}
