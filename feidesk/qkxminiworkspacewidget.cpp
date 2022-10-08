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

#include "qkxminiworkspacewidget.h"
#include "ui_qkxminiworkspacewidget.h"
#include "qkxpowerapplication.h"
#include "qkxminisessionitem.h"
#include "qkxnetclient.h"
#include "qkxnet_share.h"

#include <QMouseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QPainter>
#include <QTimer>
#include <QStyleOption>

QKxMiniWorkspaceWidget::QKxMiniWorkspaceWidget(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint | Qt::Tool),
    ui(new Ui::QKxMiniWorkSpaceWidget)
{
    ui->setupUi(this);
    setObjectName("workspace");
    setAttribute(Qt::WA_DeleteOnClose);
    ui->btnClose->setObjectName("workspace");
    QObject::connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(onAboutToClose()));

    m_sessionLayout = new QVBoxLayout(ui->content);
    ui->content->setLayout(m_sessionLayout);

    setFixedSize(240, 180);
    resetLayout();
}

QKxMiniWorkspaceWidget::~QKxMiniWorkspaceWidget()
{
    delete ui;
}

void QKxMiniWorkspaceWidget::addSession(qint64 did, const QVariantMap &dm)
{
    QKxMiniSessionItem *item = new QKxMiniSessionItem(did, dm, ui->content);
    m_sessionLayout->addWidget(item);
    m_sessions.insert(did, item);
    QObject::connect(item, SIGNAL(aboutToClose(qint64)), this, SLOT(onItemAboutToClose(qint64)));
    resetLayout();
}

void QKxMiniWorkspaceWidget::removeSession(qint64 did)
{
    QKxMiniSessionItem* item = m_sessions.take(did);
    if(item != nullptr) {
        m_sessionLayout->removeWidget(item);
        item->deleteLater();
        resetLayout();
    }

    if(m_sessions.isEmpty()) {
        close();
    }
}

void QKxMiniWorkspaceWidget::addService(qint64 did, qint8 type)
{
    QKxMiniSessionItem* item = m_sessions.value(did);
    if(item != nullptr) {
        item->addService(type-1);
    }
}

void QKxMiniWorkspaceWidget::removeService(qint64 did, qint8 type)
{
    QKxMiniSessionItem* item = m_sessions.value(did);
    if(item != nullptr) {
        item->removeService(type);
    }
}

bool QKxMiniWorkspaceWidget::sessionEmpty()
{
    return m_sessions.isEmpty();
}

void QKxMiniWorkspaceWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void QKxMiniWorkspaceWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void QKxMiniWorkspaceWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void QKxMiniWorkspaceWidget::onAboutToClose()
{
    QKxNetClient *net = QKxPowerApplication::netClient();
    for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
        qint64 did = it.key();
        net->sendPeerMessage(did, PEER_MESSAGE_MANUAL_CLOSE_PEER, "close peer");
        net->forceToClosePeer(did);
    }
    close();
}

void QKxMiniWorkspaceWidget::onItemAboutToClose(qint64 devid)
{
    removeSession(devid);
}

void QKxMiniWorkspaceWidget::onResetLayout()
{
    QSize sz = size();
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rt = screen->geometry();
    rt.setLeft(rt.right() - sz.width());
    rt.setTop(rt.bottom() - sz.height());
    setGeometry(rt);
}

void QKxMiniWorkspaceWidget::resetLayout()
{
    QMetaObject::invokeMethod(this, "onResetLayout", Qt::QueuedConnection);
}
