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

#include "qkxtermstatusform.h"
#include "ui_qkxtermstatusform.h"
#include "qkxloadingbarwidget.h"

#include <QDateTime>
#include <QDebug>

QKxTermStatusForm::QKxTermStatusForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxTermStatusForm)
    , m_lastSplash(0)
{
    ui->setupUi(this);
    setCursor(Qt::ArrowCursor);
    m_loading = new QKxLoadingBarWidget(Qt::black, this);
    m_loading->setFixedHeight(5);
    m_loading->setSpeed(5);
    ui->btnTry->setVisible(false);
    ui->btnTry->setObjectName("termTry");
    ui->showArea->setSpacing(0);
    ui->showArea->insertWidget(1, m_loading);
    ui->msgArea->setObjectName("termMsgArea");
    ui->msg->setText(tr("connecting..."));
    ui->msg->setObjectName("termMsg");
    QObject::connect(ui->btnTry, SIGNAL(clicked(bool)), SIGNAL(tryAgain()));
}

QKxTermStatusForm::~QKxTermStatusForm()
{
    delete ui;
}

void QKxTermStatusForm::setSplashMessage(const QString &msg)
{
    ui->msg->setText(msg);
    m_lastSplash = QDateTime::currentSecsSinceEpoch();
}

void QKxTermStatusForm::setMessage(const QString &msg)
{
    qint64 now = QDateTime::currentSecsSinceEpoch();
    if(now - m_lastSplash < 1) {
        return;
    }
    ui->msg->setText(msg);
}

void QKxTermStatusForm::setTryButtonVisible(bool on)
{
    ui->btnTry->setVisible(on);
}
