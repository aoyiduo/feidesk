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

#include "qkxoptionsdialog.h"
#include "ui_qkxoptionsdialog.h"

#include "qkxsettingex.h"
#include "qkxutils.h"

#include <QTimer>
#include <QDir>
#include <QStringListModel>
#include <QTranslator>
#include <QKeySequence>
#include <QVariant>
#include <QTextCodec>
#include <QStandardItemModel>
#include <QTreeView>
#include <QModelIndex>

#define OPTION_TYPE                     (Qt::UserRole+101)
#define OPTION_TYPE_NETWORK             (1)
#define OPTION_TYPE_REMOTE_DESKTOP      (2)

QKxOptionsDialog::QKxOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QKxOptionsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Global Options"));
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags &~Qt::WindowContextHelpButtonHint);
    m_flags = 0;
    QStandardItemModel *model = new QStandardItemModel(this);
    {
        QStandardItem *network = new QStandardItem(tr("Network"));
        network->setData(OPTION_TYPE_NETWORK, OPTION_TYPE);
        model->appendRow(network);
    }
    {
        QStandardItem *desktop = new QStandardItem(tr("RemoteDesktop"));
        desktop->setData(OPTION_TYPE_REMOTE_DESKTOP, OPTION_TYPE);
        //QStandardItem *mouse = new QStandardItem(tr("LocalMouse"));
        //desktop->appendRow(mouse);
        model->appendRow(desktop);
    }
    ui->type->setModel(model);
    ui->type->expandAll();
    ui->type->setMaximumWidth(160);
    ui->type->setIndentation(8);
    ui->type->setEditTriggers(QTreeView::NoEditTriggers);
    QObject::connect(ui->type, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTreeItemClicked(const QModelIndex&)));

    initDefault();

    QObject::connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(onButtonSaveClicked()));
    QObject::connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

QKxOptionsDialog::~QKxOptionsDialog()
{
    delete ui;
}

QFlags<QKxOptionsDialog::EChangeFlag> QKxOptionsDialog::result()
{
    return m_flags;
}

void QKxOptionsDialog::initDefault()
{
    {
        int mode = QKxSettingEx::networkConnection();
        if(mode == 0) {
            ui->netTcpForce->setChecked(true);
        }else{
            ui->netUdpProirity->setChecked(true);
        }
    }
    {
        int mode = QKxSettingEx::networkP2PProirity();
        if(mode == 0) {
            ui->p2pRelay->setChecked(true);
        }else{
            ui->p2pDirect->setChecked(true);
        }
    }
    {
        bool show = QKxSettingEx::desktopLocalMouseShow();
        ui->localMouse->setChecked(show);
    }
}

void QKxOptionsDialog::onButtonSaveClicked()
{
    m_flags = 0;
    {
        int mode = 0;
        if(ui->netUdpProirity->isChecked()) {
            mode = 1;
        }
        int modeOld = QKxSettingEx::networkConnection();
        QKxSettingEx::setNetworkConnection(mode);
        if(mode != modeOld) {
            m_flags |= ECF_Connection;
        }
    }
    {
        int mode = 0;
        if(ui->p2pDirect->isChecked()) {
            mode = 1;
        }
        int modeOld = QKxSettingEx::networkP2PProirity();
        QKxSettingEx::setNetworkP2PProirity(mode);
        if(mode != modeOld) {
            m_flags |= ECF_P2PMode;
        }
    }
    {
        bool show = ui->localMouse->isChecked();
        QKxSettingEx::setDesktopLocalMouseShow(show);
    }
    done(QDialog::Accepted);
}

void QKxOptionsDialog::onTreeItemClicked(const QModelIndex &idx)
{
    int t = idx.data(OPTION_TYPE).toInt();
    if(t == OPTION_TYPE_NETWORK) {
        ui->stacked->setCurrentWidget(ui->network);
    }else if(t == OPTION_TYPE_REMOTE_DESKTOP) {
        ui->stacked->setCurrentWidget(ui->desktop);
    }
}
