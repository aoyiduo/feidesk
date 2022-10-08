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

#include "qkxlanguagedialog.h"
#include "ui_qkxlanguagedialog.h"
#include "qkxsettingex.h"

#include <QTimer>
#include <QTranslator>
#include <QCoreApplication>

QKxLanguageDialog::QKxLanguageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxLanguageDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Language Configure"));

    QObject::connect(ui->btnSave, SIGNAL(clicked(bool)), this, SLOT(onButtonSaveClicked()));

    QString name = QKxSettingEx::languageName();
    if(name == "Chinese") {
        ui->chkZh->setChecked(true);
    }else{
        ui->chkEn->setChecked(true);
    }
    setMinimumWidth(250);

    QTimer::singleShot(0, this, [=]{
        QSize sz = minimumSize();
        resize(sz);
    });
}

QKxLanguageDialog::~QKxLanguageDialog()
{
    delete ui;
}

void QKxLanguageDialog::onButtonSaveClicked()
{
    QString name = QKxSettingEx::languageName();
    QString lang;
    if(ui->chkEn->isChecked()) {
        lang = "English";
    }else{
        lang = "Chinese";
    }    
    if(name != lang) {
        QKxSettingEx::setLanguageName(lang);
        done(QDialog::Accepted);
    }else{
        done(QDialog::Rejected);
    }
}
