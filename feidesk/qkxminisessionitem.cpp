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

#include "qkxminisessionitem.h"
#include "ui_qkxminisessionitem.h"
#include "qkxnetclient.h"
#include "qkxpowerapplication.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

QKxMiniSessionItem::QKxMiniSessionItem(qint64 deviceId, const QVariantMap &data, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxMiniSessionItem)
    , m_deviceId(deviceId)
    , m_data(data)
{
    ui->setupUi(this);

    setObjectName("miniItem");

    ui->btnClose->setObjectName("workspace");
    QString friendlyName = data.value("FriendlyName").toString();
    if(friendlyName.isEmpty()) {
        friendlyName = data.value("nickName").toString();
        if(friendlyName.isEmpty()) {
            friendlyName = data.value("loginName").toString();
        }
    }
    ui->friendlyName->setObjectName("miniFriendlyName");
    ui->friendlyName->setText(friendlyName);
    m_tools.append(ui->btnDesktop);
    m_tools.append(ui->btnFtp);
    m_tools.append(ui->btnChat);
    m_tools.append(ui->btnTerminal);
    m_tools.append(ui->btnPrinter);
    m_tools.append(ui->btnCamera);
    ui->btnDesktop->setObjectName("workspace");
    ui->btnFtp->setObjectName("workspace");
    ui->btnChat->setObjectName("workspace");

    QObject::connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));

    for(int i = 0; i < m_tools.length(); i++) {
        QToolButton *btn = m_tools.at(i);
        btn->hide();
    }
}

QKxMiniSessionItem::~QKxMiniSessionItem()
{
    delete ui;
}

void QKxMiniSessionItem::addService(qint8 type)
{
    if(type < 0 || type >= m_tools.length()) {
        return;
    }
    m_tools.at(type)->show();
}

void QKxMiniSessionItem::removeService(qint8 type)
{
    if(type < 0 || type >= m_tools.length()) {
        return;
    }
    m_tools.at(type)->hide();
}

void QKxMiniSessionItem::paintEvent(QPaintEvent *e)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void QKxMiniSessionItem::onCloseButtonClicked()
{
    QKxNetClient *net = QKxPowerApplication::netClient();
    net->sendPeerMessage(m_deviceId, PEER_MESSAGE_MANUAL_CLOSE_PEER, "close peer");
    net->forceToClosePeer(m_deviceId);
    emit aboutToClose(m_deviceId);
}
