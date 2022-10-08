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

#include "qkxconnectionform.h"
#include "ui_qkxconnectionform.h"
#include "qkxsettingex.h"
#include "qkxbuttonassist.h"
#include "qkxutils.h"
#include "qkxservicemanager.h"
#include "qkxlocalabilitymodel.h"
#include "qkxiconframepaint.h"
#include "qkxnet_share.h"
#include "qkxpushbutton.h"
#include "qkxpowerapplication.h"
#include "qkxnetclient.h"
#include "qkxmodel_share.h"
#include "qkxauthorizeconfirmdialog.h"
#include "qkxmydeviceform.h"
#include "qkxaccount.h"

#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
#include <QRegExp>
#include <QInputDialog>

#define SERVICE_TYPE    ("serviceType")

QKxConnectionForm::QKxConnectionForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QKxConnectionForm)
{
    ui->setupUi(this);
    {
        ui->deviceID->setReadOnly(true);
        qint64 did = QKxSettingEx::lastDeviceID();
        ui->deviceID->setText(QString("%1").arg(did));
        QKxButtonAssist *assist = new QKxButtonAssist(":/resource/skin/copy.png", ui->deviceID);
        QObject::connect(assist, SIGNAL(clicked(int)), this, SLOT(onDeviceIDAction(int)));
    }
    {
        ui->devicePassword->setText("");
        ui->devicePassword->setReadOnly(true);
        QKxButtonAssist *assist = new QKxButtonAssist(":/resource/skin/reload.png", ui->devicePassword);
        assist->append(":/resource/skin/copy.png");
        QObject::connect(assist, SIGNAL(clicked(int)), this, SLOT(onDevicePasswordAction(int)));
    }
    {
        QKxLocalAbilityModel *model = new QKxLocalAbilityModel(this);
        ui->localAbility->setModel(model);
        ui->localAbility->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->localAbility->setItemDelegate(new QKxIconFramePaint(ui->localAbility));
        ui->localAbility->setViewMode(QListView::IconMode);
        ui->localAbility->setResizeMode(QListView::Adjust);
        ui->localAbility->setSpacing(0);
        QObject::connect(ui->localAbility, SIGNAL(clicked(QModelIndex)), this, SLOT(onAbilityItemClicked(QModelIndex)));
        QObject::connect(ui->localAbility, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAbilityItemDbClicked(QModelIndex)));
        QObject::connect(ui->btnStart, SIGNAL(clicked(bool)), this, SLOT(onLocalAbilityStartClicked()));
        QObject::connect(ui->btnStop, SIGNAL(clicked(bool)), this, SLOT(onLocalAbilityStopClicked()));
        m_serviceManager = new QKxServiceManager(this);
        QObject::connect(m_serviceManager, SIGNAL(serviceReset(quint64)), model, SLOT(onServiceFlagActive(quint64)));
        QObject::connect(m_serviceManager, SIGNAL(serviceReset(quint64)), this, SLOT(onServiceFlagActive(quint64)));
    }
    {
        ui->btnIDConnect->setProperty(SERVICE_TYPE, 1);
        QObject::connect(ui->btnIDConnect, SIGNAL(clicked(bool)), this, SLOT(onIDConnectClicked()));
        QObject::connect(ui->btnIDConnectOption, SIGNAL(clicked(bool)), this, SLOT(onIDConnectOptionClicked()));
    }
    {
        QObject::connect(ui->btnAuthorize, SIGNAL(clicked(bool)), this, SLOT(onAuthorizeButtonClicked()));
    }
}

QKxConnectionForm::~QKxConnectionForm()
{
    delete ui;
}

void QKxConnectionForm::setDeviceID(qint64 did)
{
    ui->deviceID->setText(QString::number(did));
    QKxSettingEx::setLastDeviceID(did);
    m_deviceId = did;
}

void QKxConnectionForm::onAuthorizeButtonClicked()
{
    auto doAuthorize = [=]() {
        if(ui->btnAuthorize->isChecked()) {
            QKxAuthorizeConfirmDialog dlg(this);
            int ret = dlg.exec();
            ui->btnAuthorize->setChecked(ret == QDialog::Accepted);
        }else{
            int ret = QMessageBox::warning(this, tr("revoking authorization"), tr("After the authorization is revoked, the device password connection will be restored"), QMessageBox::Yes|QMessageBox::No);
            if(ret != QMessageBox::Yes) {
                ui->btnAuthorize->setChecked(true);
                return;
            }
            ui->btnAuthorize->setChecked(false);
        }
    };
    doAuthorize();
    syncAuthorize();
}

void QKxConnectionForm::onDeviceIDAction(int idx)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->deviceID->text());
}

void QKxConnectionForm::onDevicePasswordAction(int idx)
{
    if(idx == 0) {
        QKxNetClient *net = QKxPowerApplication::netClient();
        QByteArray pass = net->generateDevicePassword();
        ui->devicePassword->setText(pass);
    }else{
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->devicePassword->text());
    }
}

void QKxConnectionForm::onAbilityItemClicked(const QModelIndex &idx)
{
    qint32 flag = idx.data(ROLE_SERVICE_RUNNING).toInt();
    bool on = flag > 0;
    ui->btnStop->setVisible(on);
    ui->btnStart->setVisible(!on);
    if(!ui->btnStart->isEnabled()) {
        ui->btnStart->setEnabled(true);
    }
    if(!ui->btnStop->isEnabled()) {
        ui->btnStop->setEnabled(true);
    }
}

void QKxConnectionForm::onAbilityItemDbClicked(const QModelIndex &idx)
{
    bool running = idx.data(ROLE_SERVICE_RUNNING).toBool();
    qint32 type = idx.data(ROLE_SERVICE_TYPE).toInt();
    QString msg = running ? tr("continue to stop the service?") : tr("continue to start the service?");
    if(type == SERVICE_FLAG_VNC) {
        int rv = QMessageBox::information(this, tr("confirm"), msg, QMessageBox::Yes|QMessageBox::No);
        if(rv != QMessageBox::Yes) {
            return;
        }
        m_serviceManager->setDesktopActive(!running);
    }else if(type == SERVICE_FLAG_FTP) {
        int rv = QMessageBox::information(this, tr("confirm"), msg, QMessageBox::Yes|QMessageBox::No);
        if(rv != QMessageBox::Yes) {
            return;
        }
        m_serviceManager->setFtpActive(!running);
    }else if(type == SERVICE_FLAG_TERMINAL) {
        int rv = QMessageBox::information(this, tr("confirm"), msg, QMessageBox::Yes|QMessageBox::No);
        if(rv != QMessageBox::Yes) {
            return;
        }
        m_serviceManager->setTerminalActive(!running);
    }else if(type == SERVICE_FLAG_CHAT) {
        int rv = QMessageBox::information(this, tr("confirm"), msg, QMessageBox::Yes|QMessageBox::No);
        if(rv != QMessageBox::Yes) {
            return;
        }
        m_serviceManager->setChatActive(!running);
    }else{
        QMessageBox::information(this, tr("Tip"), tr("this function is not open yet"));
    }
}

void QKxConnectionForm::onLocalAbilityStartClicked()
{
    QItemSelectionModel *model = ui->localAbility->selectionModel();
    QModelIndexList lstIdx = model->selectedRows();
    if(lstIdx.isEmpty()) {
        QMessageBox::information(this, tr("tip"), tr("Please select an item to click."));
        return;
    }
    QModelIndex idx = lstIdx.at(0);
    qint32 flag = idx.data(ROLE_SERVICE_TYPE).toInt();
    if(flag == SERVICE_FLAG_VNC) {
        m_serviceManager->setDesktopActive(true);
    }else if(flag == SERVICE_FLAG_FTP) {
        m_serviceManager->setFtpActive(true);
    }else{
        QMessageBox::information(this, tr("Tip"), tr("this function is not open yet"));
    }
}

void QKxConnectionForm::onLocalAbilityStopClicked()
{
    QItemSelectionModel *model = ui->localAbility->selectionModel();
    QModelIndexList lstIdx = model->selectedRows();
    if(lstIdx.isEmpty()) {
        QMessageBox::information(this, tr("tip"), tr("Please select an item to click."));
        return;
    }
    QModelIndex idx = lstIdx.at(0);
    qint32 flag = idx.data(ROLE_SERVICE_TYPE).toInt();
    if(flag == SERVICE_FLAG_VNC) {
        m_serviceManager->setDesktopActive(false);
    }else if(flag == SERVICE_FLAG_FTP) {
        m_serviceManager->setFtpActive(false);
    }else{
        QMessageBox::information(this, tr("Tip"), tr("this function is not open yet"));
    }
}

void QKxConnectionForm::onIDConnectClicked()
{
    QString name = ui->friendlyName->text();
    if(name.isEmpty()) {
        QMessageBox::information(this, tr("Failure"), tr("Please input a friendly name to mark who you are."));
        return;
    }
    QString txt = ui->remoteDeviceID->text();
    if(txt.isEmpty()) {
        QMessageBox::information(this, tr("Failure"), tr("Please input a remote device id."));
        return;
    }
    QRegExp rgx("\\d+");
    if(!rgx.exactMatch(txt)) {
        QMessageBox::information(this, tr("Failure"), tr("Please enter the correct device ID."));
        return;
    }
    qint64 remoteId = txt.toLongLong();

    if(remoteId == m_deviceId) {
        QMessageBox::information(this, tr("Failure"), tr("Can not connect to local device."));
        return;
    }
    QString password = ui->remoteDevicePassword->text();
    if(password.isEmpty()) {
        QMessageBox::information(this, tr("information"), tr("Please input a connection password."));
        return;
    }
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    int type = btn->property(SERVICE_TYPE).toInt();
    qDebug() << "onIDConnectClicked" << type;
    QMetaObject::invokeMethod(QKxPowerApplication::instance(),
                              "openRemoteService",
                              Qt::QueuedConnection,
                              Q_ARG(qint64, remoteId),
                              Q_ARG(int, type),
                              Q_ARG(QByteArray, name.toLatin1()),
                              Q_ARG(QByteArray, password.toLatin1()),
                              Q_ARG(bool, false));
}

void QKxConnectionForm::onIDConnectOptionClicked()
{
    QMenu menu(this);
    {
        QAction *tmp = menu.addAction(QIcon(":/resource/skin/desktop.png"), tr("Desktop"), this, SLOT(onIDConnectOptionMenuClicked()));
        tmp->setProperty(SERVICE_TYPE, 1);
    }
    {
        QAction *tmp = menu.addAction(QIcon(":/resource/skin/ftp.png"), tr("Ftp"), this, SLOT(onIDConnectOptionMenuClicked()));
        tmp->setProperty(SERVICE_TYPE, 2);
    }
    {
        QAction *tmp = menu.addAction(QIcon(":/resource/skin/terminal.png"), tr("Terminal"), this, SLOT(onIDConnectOptionMenuClicked()));
        tmp->setProperty(SERVICE_TYPE, 4);
    }
    menu.exec(QCursor::pos());
}

void QKxConnectionForm::onIDConnectOptionMenuClicked()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString txt = action->text()+tr("Connect");
    ui->btnIDConnect->setText(txt);
    ui->btnIDConnect->setProperty(SERVICE_TYPE, action->property(SERVICE_TYPE));
}

void QKxConnectionForm::onServiceFlagActive(quint64 flag)
{
    QKxNetClient *netClient = QKxPowerApplication::netClient();
    netClient->updateServiceFlag(flag);
}

void QKxConnectionForm::syncAuthorize()
{
    QKxNetClient *net = QKxPowerApplication::netClient();
    QVariantMap vm;
    if(ui->btnAuthorize->isChecked()) {
        QString authName = QKxSettingEx::authorizeLoginName();
        vm.insert("loginName", authName);
    }
    net->updateAuthorize(vm);
}

void QKxConnectionForm::onAccountLoginResult(qint32 err, qint32 okcnt, const QString &msg)
{
    QKxAccount *account = QKxMyDeviceForm::loginAccount();
    if(account != nullptr && account->hasLogin()) {
        QString loginName = account->loginName();
        QString authName = QKxSettingEx::authorizeLoginName();
        ui->btnAuthorize->setChecked(loginName == authName);
    }else{
        ui->btnAuthorize->setChecked(false);
    }
    syncAuthorize();
}

void QKxConnectionForm::onLogoutArrived()
{
    ui->btnAuthorize->setChecked(false);
    syncAuthorize();
}

void QKxConnectionForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    ui->btnStart->show();
    ui->btnStop->show();
    ui->btnStop->setEnabled(false);
    ui->btnStart->setEnabled(false);
    if(m_serviceManager) {
        m_serviceManager->checkServiceStatus();
    }
}

void QKxConnectionForm::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}
