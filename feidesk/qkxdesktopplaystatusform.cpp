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

#include "qkxdesktopplaystatusform.h"
#include "ui_qkxdesktopplaystatusform.h"
#include "qkxloadingbarwidget.h"

#include <QDateTime>
#include <QDebug>

QKxDesktopPlayStatusForm::QKxDesktopPlayStatusForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxDesktopPlayStatusForm)
    , m_lastSplash(0)
{
    ui->setupUi(this);
    setCursor(Qt::ArrowCursor);
    m_loading = new QKxLoadingBarWidget(Qt::lightGray, this);
    m_loading->setFixedHeight(5);
    m_loading->setSpeed(5);
    ui->btnTry->setVisible(false);
    ui->btnTry->setObjectName("deskTry");
    ui->showArea->setSpacing(0);
    ui->showArea->insertWidget(1, m_loading);
    ui->msgArea->setObjectName("deskMsgArea");
    ui->msg->setText(tr("connecting..."));
    ui->msg->setObjectName("deskMsg");
    QObject::connect(ui->btnTry, SIGNAL(clicked(bool)), SIGNAL(tryAgain()));
}

QKxDesktopPlayStatusForm::~QKxDesktopPlayStatusForm()
{
    delete ui;
}

void QKxDesktopPlayStatusForm::setSplashMessage(const QString &msg)
{
    ui->msg->setText(msg);
    m_lastSplash = QDateTime::currentSecsSinceEpoch();
}

void QKxDesktopPlayStatusForm::setMessage(const QString &msg)
{
    qint64 now = QDateTime::currentSecsSinceEpoch();
    if(now - m_lastSplash < 1) {
        return;
    }
    ui->msg->setText(msg);
}

void QKxDesktopPlayStatusForm::setTryButtonVisible(bool on)
{
    ui->btnTry->setVisible(on);
}

void QKxDesktopPlayStatusForm::showEvent(QShowEvent *event)
{
    emit showArrived();
}
