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

#include "qkxdesktopftpwidget.h"
#include "ui_qkxdesktopftpwidget.h"

#include "qkxvncwidget.h"
#include "qkxftptransferwidget.h"
#include "qkxftprequest.h"
#include "qkxdesktopftprequest.h"
#include "qkxsettingex.h"

#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QDebug>


QKxDesktopFtpWidget::QKxDesktopFtpWidget(const QString& devid, QKxVncWidget *vnc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QKxDesktopFtpWidget)
    , m_vnc(vnc)
{
    hide();
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->btnClose->setObjectName("btnClose");
    QObject::connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(onAboutToClose()));

    QObject::connect(vnc, SIGNAL(finished()), this, SLOT(onFinished()));

    QString path = QKxSettingEx::ensurePath("task");
    m_transfer = new QKxFtpTransferWidget(path, devid, this);
    QBoxLayout *lo = qobject_cast<QBoxLayout*>(layout());
    lo->addWidget(m_transfer);

    QKxDesktopFtpRequest *ftp = new QKxDesktopFtpRequest(vnc, this);
    m_transfer->setRequest(ftp);
    m_ftpRequest = ftp;
}

QKxDesktopFtpWidget::~QKxDesktopFtpWidget()
{
    delete ui;
}

void QKxDesktopFtpWidget::onAboutToClose()
{
    close();
}

void QKxDesktopFtpWidget::onFinished()
{
    close();
}

void QKxDesktopFtpWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void QKxDesktopFtpWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void QKxDesktopFtpWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
