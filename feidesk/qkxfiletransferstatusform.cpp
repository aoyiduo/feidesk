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

#include "qkxfiletransferstatusform.h"
#include "ui_qkxfiletransferstatusform.h"
#include "qkxloadingbarwidget.h"

#include <QDateTime>
#include <QDebug>

QKxFileTransferStatusForm::QKxFileTransferStatusForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxFileTransferStatusForm)
    , m_lastSplash(0)
{
    ui->setupUi(this);
    setCursor(Qt::ArrowCursor);
    m_loading = new QKxLoadingBarWidget(Qt::black, this);
    m_loading->setFixedHeight(5);
    m_loading->setSpeed(5);
    ui->btnTry->setVisible(false);
    ui->btnTry->setObjectName("ftpTry");
    ui->showArea->setSpacing(0);
    ui->showArea->insertWidget(1, m_loading);
    ui->msgArea->setObjectName("ftpMsgArea");
    ui->msg->setText(tr("connecting..."));
    ui->msg->setObjectName("ftpMsg");
    QObject::connect(ui->btnTry, SIGNAL(clicked(bool)), SIGNAL(tryAgain()));
}

QKxFileTransferStatusForm::~QKxFileTransferStatusForm()
{
    delete ui;
}

void QKxFileTransferStatusForm::setSplashMessage(const QString &msg)
{
    ui->msg->setText(msg);
    m_lastSplash = QDateTime::currentSecsSinceEpoch();
}

void QKxFileTransferStatusForm::setMessage(const QString &msg)
{
    qint64 now = QDateTime::currentSecsSinceEpoch();
    if(now - m_lastSplash < 1) {
        return;
    }
    ui->msg->setText(msg);
}

void QKxFileTransferStatusForm::setTryButtonVisible(bool on)
{
    ui->btnTry->setVisible(on);
}
