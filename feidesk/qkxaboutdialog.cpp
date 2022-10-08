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

#include "qkxaboutdialog.h"
#include "ui_qkxaboutdialog.h"
#include "version.h"

#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

QKxAboutDialog::QKxAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxAboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("About..."));

    ui->version->setText(tr("FeiDesk")+"  v" + FEIDESK_VERSION);
    ui->copyright->setText("(c)2022-2023 AoYiDuo Inc");
    ui->web->setText("<a href=\"http://www.aoyiduo.com/feidesk/\">http://www.aoyiduo.com/feidesk</a>");
    ui->web->setTextFormat(Qt::RichText);
    ui->web->setOpenExternalLinks(false);
    ui->web->setTextInteractionFlags(Qt::TextBrowserInteraction);
    QObject::connect(ui->web, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)));

    QObject::connect(ui->btnOk, SIGNAL(clicked(bool)), this, SLOT(close()));

    QTimer::singleShot(0, this, [=]{
        QSize sz = minimumSize();
        resize(sz);
    });

}

QKxAboutDialog::~QKxAboutDialog()
{
    delete ui;
}

void QKxAboutDialog::onLinkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
