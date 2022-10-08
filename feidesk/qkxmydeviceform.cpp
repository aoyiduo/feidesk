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

#include "qkxmydeviceform.h"
#include "ui_qkxmydeviceform.h"
#include "qkxbuttonassist.h"
#include "qkxutils.h"
#include "qkxsettingex.h"
#include "qkxpowerapplication.h"
#include "qkxpowermainwindow.h"
#include "qkxaccount.h"
#include "qtservice.h"
#include "qkxdeskdef.h"
#include "qkxnet_share.h"
#include "qkxnetclient.h"
#include "qkxdevicetreemodel.h"
#include "qkxremoteabilitymodel.h"
#include "qkxnet_share.h"
#include "qkxiconframepaint.h"
#include "qkxusercreatedialog.h"
#include "qkxmodel_share.h"

#include <QClipboard>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>
#include <QVBoxLayout>
#include <QMenu>
#include <QSignalMapper>
#include <QDesktopServices>
#include <QUrl>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QItemSelectionModel>
#include <QMessageBox>

#define PASSWORD_INIT_TEXT  ("1*2*3*4*")

class QKxTreeSpecialPaint : public QStyledItemDelegate
{
public:
    explicit QKxTreeSpecialPaint(QWidget *parent = 0)
        : QStyledItemDelegate(parent) {

    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &idx) const {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, idx);
        opt.decorationSize = QSize(24, 24);
        const QWidget *widget = NULL;
        widget = option.widget;
        QStyle *style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    }
};

QKxMyDeviceForm::QKxMyDeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QKxMyDeviceForm)
{
    ui->setupUi(this);
    {
        ui->userPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        m_pwdTimer = new QTimer(this);
        QObject::connect(m_pwdTimer, SIGNAL(timeout()), this, SLOT(onAutoSetPasswordMode()));
        m_pwdTimer->setInterval(1000 * 3);
        ui->userPassword->installEventFilter(this);
        QObject::connect(ui->userPassword, SIGNAL(textChanged(QString)), this, SLOT(onUserPasswordChanged(QString)));
        QKxButtonAssist *assist = new QKxButtonAssist(":/resource/skin/eye.png", ui->userPassword);
        QObject::connect(assist, SIGNAL(clicked(int)), this, SLOT(onUserPasswordAction(int)));
        assist->setEnabled(0, false);
        m_passVisible = assist;
    }
    {
        ui->accountRegistry->setTextFormat(Qt::RichText);
        ui->accountRegistry->setOpenExternalLinks(false);
        ui->accountRegistry->setTextInteractionFlags(Qt::TextBrowserInteraction);
        QObject::connect(ui->accountRegistry, SIGNAL(linkActivated(QString)), this, SLOT(onAccountLinkActivated(QString)));
        ui->accountRegistry->setText(tr("Login to the official website to manage your devices")
                                     + QString(" >>> <a href=\"http://%1\">http://%1<a/>").arg(MY_DEVICE_MANAGE_HOST));
        QObject::connect(ui->btnRegister, SIGNAL(clicked(bool)), this, SLOT(onAccountToRegister()));
        ui->accountRegistry->hide();
    }
    {
        QObject::connect(ui->btnLogin, SIGNAL(clicked(bool)), this, SLOT(onTryToLogin()));
        QObject::connect(ui->chkLogin, SIGNAL(clicked(bool)), this, SLOT(onCheckLoginButtonClicked()));
    }
    if(QKxSettingEx::rememberAccount()) {
        ui->chkRemember->setChecked(true);
        QString name = QKxSettingEx::lastUserName();
        ui->userName->setText(name);
        ui->userPassword->setText(PASSWORD_INIT_TEXT);
        if(QKxSettingEx::autoLogin()) {
            ui->chkLogin->setChecked(true);
        }
    }else{
        ui->userName->setText("");
        ui->userPassword->setText("");
    }
    {
        m_deviceModel = new QKxDeviceTreeModel(this);
        ui->deviceTree->setModel(m_deviceModel);
        ui->deviceTree->setHeaderHidden(true);
        ui->deviceTree->setIndentation(5);
        ui->deviceTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->deviceTree->setItemDelegate(new QKxTreeSpecialPaint(ui->deviceTree));
        QObject::connect(m_deviceModel, SIGNAL(hiddenArrived(int,bool)), this, SLOT(onTreeHiddenArrived(int,bool)), Qt::QueuedConnection);
        QObject::connect(ui->deviceTree, SIGNAL(clicked(QModelIndex)), this, SLOT(onDeviceListItemClicked(QModelIndex)));
    }
    {
        QKxRemoteAbilityModel *model = new QKxRemoteAbilityModel(this);
        m_remoteModel = model;
        ui->remoteAbility->setModel(model);
        ui->remoteAbility->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->remoteAbility->setItemDelegate(new QKxIconFramePaint(ui->remoteAbility));
        ui->remoteAbility->setViewMode(QListView::IconMode);
        ui->remoteAbility->setResizeMode(QListView::Adjust);
        ui->remoteAbility->setSpacing(0);
        QObject::connect(ui->remoteAbility, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onRemoteAbilityDoubleClicked(QModelIndex)));
        QObject::connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(onRemoteAbilityConnectClicked()));
    }
}

QKxMyDeviceForm::~QKxMyDeviceForm()
{
    delete ui;
}

QKxAccount *QKxMyDeviceForm::loginAccount()
{
    return QKxPowerMainWindow::myDeviceForm()->m_account;
}

void QKxMyDeviceForm::logout()
{
    exitLogin();
    emit logoutArrived();
}

void QKxMyDeviceForm::cleanAccount()
{
    ui->userName->clear();
    ui->userPassword->clear();
    ui->chkLogin->setChecked(false);
    ui->chkRemember->setChecked(false);
}

void QKxMyDeviceForm::onTreeHiddenArrived(int row, bool on)
{
    QModelIndex root = ui->deviceTree->rootIndex();
    bool isHidden = ui->deviceTree->isRowHidden(row, root);
    if(isHidden != on) {
        ui->deviceTree->setRowHidden(row, root, on);
    }
    if(!on) {
        QTimer::singleShot(1000, this, SLOT(onTreeDeviceExpandAll()));
    }
}

void QKxMyDeviceForm::onTreeDeviceExpandAll()
{
    ui->deviceTree->expandAll();
    selectFirstChild();
}

void QKxMyDeviceForm::onUserPasswordAction(int idx)
{
    QLineEdit::EchoMode mode = ui->userPassword->echoMode();
    if(mode == QLineEdit::PasswordEchoOnEdit) {
        ui->userPassword->setEchoMode(QLineEdit::Normal);
    }else{
        ui->userPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    }
}

void QKxMyDeviceForm::onAutoSetPasswordMode()
{
    ui->userPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_pwdTimer->stop();
    QString txt = ui->userPassword->text();
    if(!txt.isEmpty()) {
        m_passVisible->setEnabled(0, false);
        ui->userPassword->setText(PASSWORD_INIT_TEXT);
    }
}

void QKxMyDeviceForm::onUserPasswordChanged(const QString &txt)
{
    qDebug() << "txt" << txt;
    if(txt == PASSWORD_INIT_TEXT || txt.isEmpty()) {
        return;
    }
    m_passVisible->setEnabled(0, true);
}

void QKxMyDeviceForm::onTryToLogin()
{
    QString name = ui->userName->text();
    if(name.isEmpty()) {
        QMessageBox::warning(this, tr("Tip"), tr("The login name should not be empty."));
        return;
    }
    if(!QKxUtils::checkEmailOrPhone(name)) {
        QMessageBox::warning(this, tr("Tip"), tr("login name should be a valid telephone number or email."));
        return;
    }
    QString pwdMd5 = QKxSettingEx::lastUserPassword();
    QString pwd = ui->userPassword->text();
    if(pwd != PASSWORD_INIT_TEXT) {
        if(pwd.length() < 6) {
            QMessageBox::warning(this, tr("Tip"), tr("The number of password characters needs to be more than 6"));
            return;
        }
        if(!QKxUtils::checkPassword(pwd)) {
            QMessageBox::warning(this, tr("Tip"), tr("The password must be latin1 character"));
            return;
        }
        pwdMd5 = QCryptographicHash::hash(pwd.toLatin1(), QCryptographicHash::Md5).toHex();
    }
    if(ui->chkRemember->isChecked()){
        QKxSettingEx::setRememberAccount(true);
        QKxSettingEx::setLastUserPassword(pwdMd5.toLatin1());
        QKxSettingEx::setLastUserName(name);
    }else{
        QKxSettingEx::setRememberAccount(false);
    }
    QKxSettingEx::setAutoLogin(ui->chkLogin->isChecked());
    startLogin(name, pwdMd5);
}

void QKxMyDeviceForm::onCheckLoginButtonClicked()
{
    if(ui->chkLogin->isChecked()) {
        ui->chkRemember->setChecked(true);
    }
    QKxSettingEx::setAutoLogin(ui->chkLogin->isChecked());
}

void QKxMyDeviceForm::onAccountLinkActivated(const QString &link)
{
    qDebug() << link;
    QDesktopServices::openUrl(QUrl(link));
}

void QKxMyDeviceForm::onAccountToRegister()
{
    QKxUserCreateDialog dlg(this);
    dlg.exec();
}

void QKxMyDeviceForm::onAccountPeerError(qint32 err, const QString &msg)
{
    qDebug() << "onAccountPeerError";
}

void QKxMyDeviceForm::onAccountConnectionError(qint32 err, const QString &msg)
{
    qDebug() << "onAccountConnectionError";
}

void QKxMyDeviceForm::onAccountLoginResult(qint32 err, qint32 okcnt, const QString &msg)
{
    qDebug() << "onAccountLoginResult" << err << msg;
    if(err == 0) {
        ui->stack->setCurrentWidget(ui->pageList);
    }else if(err == -2 && okcnt > 0) {
        logout();
    }else if(err == -6 && okcnt > 0) {
        QMessageBox::warning(this, tr("login failure"), tr("device has been disabled."));
        ui->stack->setCurrentWidget(ui->pageLogin);
    }else {
        QString err1 = tr("user name or password is not right."); // msg from server and record for multi-language.
        if(okcnt == 0) {
            QMessageBox::warning(this, tr("login failure"), tr(msg.toUtf8()));
            ui->stack->setCurrentWidget(ui->pageLogin);
        }
    }
    emit loginResult(err, okcnt, msg);
}

void QKxMyDeviceForm::onRemoteFileTransfer()
{

}

void QKxMyDeviceForm::onRemoteTerminalOpen()
{

}

void QKxMyDeviceForm::onRemoteConnectionCheck()
{

}

void QKxMyDeviceForm::onRemoteDesktopOpen()
{

}

void QKxMyDeviceForm::onAutoStartLogin()
{
    if(m_account) {
        return;
    }
    QString name = QKxSettingEx::lastUserName();
    QString pwdMd5 = QKxSettingEx::lastUserPassword();
    startLogin(name, pwdMd5);
}

void QKxMyDeviceForm::onRemoteAbilityDoubleClicked(const QModelIndex &idx)
{
    qDebug() << "onRemoteAbilityDoubleClicked" << idx.row() << idx.column();
    qint64 did = idx.data(ROLE_DEVICE_ID).toLongLong();
    if(did <= 0) {
        return;
    }
    int type = idx.data(ROLE_SERVICE_TYPE).toInt();
    bool isActive = idx.data(ROLE_SERVICE_RUNNING).toBool();
    if(!isActive) {
        QMessageBox::warning(this, tr("Service activation"), tr("the remote peer has stop this service"));
        return;
    }
    QString name = ui->userName->text();
    QMetaObject::invokeMethod(QKxPowerApplication::instance(),
                              "openRemoteService",
                              Qt::QueuedConnection,
                              Q_ARG(qint64, did),
                              Q_ARG(int, type),
                              Q_ARG(QByteArray, name.toUtf8()),
                              Q_ARG(QByteArray, QByteArray()),
                              Q_ARG(bool, true));
}

void QKxMyDeviceForm::onRemoteAbilityConnectClicked()
{
    QItemSelectionModel *model = ui->remoteAbility->selectionModel();
    QModelIndexList lstIdx = model->selectedRows();
    if(lstIdx.isEmpty()) {
        QMessageBox::information(this, tr("tip"), tr("Please select an item to click."));
        return;
    }
    onRemoteAbilityDoubleClicked(lstIdx.at(0));
}

void QKxMyDeviceForm::onDeviceListItemClicked(const QModelIndex &idx)
{
    int category = idx.data(ROLE_CATEGORY).toInt();
    if(category > 0) {
        return;
    }
    int online = idx.data(ROLE_ONLINE).toInt();
    if(online > 1) {
        // my computer.
        ui->abilityStack->setCurrentWidget(ui->localPage);
    }else if(online == 1) {
        qint64 devid = idx.data(ROLE_DEVICE_ID).toUInt();
        qint64 flag = idx.data(ROLE_SERVICE_FLAG).toLongLong();
        QString name = idx.data().toString();
        m_remoteModel->setServiceFlag(devid, flag);
        ui->remoteName->setText(name);
        ui->remoteId->setText(QString("%1").arg(devid));
        ui->abilityStack->setCurrentWidget(ui->onlinePage);
    }else if(online == 0) {
        ui->abilityStack->setCurrentWidget(ui->offlinePage);
    }
}

void QKxMyDeviceForm::onReconnecting()
{

}

bool QKxMyDeviceForm::eventFilter(QObject *w, QEvent *e)
{
    QEvent::Type t = e->type();
    if(t == QEvent::FocusIn) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(w);
        if(edit != nullptr) {
            QString txt = edit->text();
            if(txt == PASSWORD_INIT_TEXT) {
                edit->setText("");
            }
            m_pwdTimer->stop();
        }
    }else if(t == QEvent::FocusOut) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(w);
        if(edit != nullptr) {
            m_pwdTimer->start();
        }
    }
    return QWidget::eventFilter(w, e);
}

void QKxMyDeviceForm::startLogin(const QString &name, const QString &pwd)
{
    QKxNetClient *net = QKxPowerApplication::netClient();
    if(!net->isServerConfigureInitialized()) {
        QMessageBox::information(this, tr("network connection"), tr("it's trying to connect to server."));
        return;
    }
    if(m_account) {
        m_account->disconnect();
        m_account->deleteLater();
    }
    m_account = net->loginAccount(name, pwd);    
    QObject::connect(m_account, SIGNAL(connectionError(qint32, QString)), this, SLOT(onAccountConnectionError(qint32,QString)));
    QObject::connect(m_account, SIGNAL(loginResult(qint32,qint32,QString)), this, SLOT(onAccountLoginResult(qint32,qint32,QString)));
    QObject::connect(m_account, SIGNAL(deviceListArrived(QVariantList)), m_deviceModel, SLOT(onDeviceListArrived(QVariantList)));
    QObject::connect(m_account, SIGNAL(deviceListArrived(QVariantList)), this, SLOT(onTreeDeviceExpandAll()), Qt::QueuedConnection);
    QObject::connect(m_account, SIGNAL(reconnecting()), this, SLOT(onReconnecting()));
    QObject::connect(m_account, SIGNAL(deviceChanged(int,QVariant)), m_deviceModel, SLOT(onDeviceChanged(int,QVariant)));
    QObject::connect(m_account, SIGNAL(serviceFlagArrived(qint64,quint64)), m_deviceModel, SLOT(onServiceFlagArrived(qint64,quint64)));
    QObject::connect(m_account, SIGNAL(serviceFlagArrived(qint64,quint64)), m_remoteModel, SLOT(onServiceFlagArrived(qint64,quint64)), Qt::QueuedConnection);
    qDebug() << "startLogin" << name << pwd;
}

void QKxMyDeviceForm::exitLogin()
{
    if(m_account != nullptr) {
        m_account->deleteLater();
    }
    ui->stack->setCurrentWidget(ui->pageLogin);
}

void QKxMyDeviceForm::selectFirstChild()
{
    QAbstractItemModel *model = ui->deviceTree->model();
    QModelIndex idx = model->index(0, 0);
    for(int i = 0; i < 10; i++) {
        QModelIndex tmp = idx.sibling(i, 0);
        if(!tmp.isValid()) {
            return;
        }
        QModelIndex child = tmp.child(0, 0);
        if(child.isValid()) {
            ui->deviceTree->selectionModel()->setCurrentIndex(child, QItemSelectionModel::SelectCurrent);
            ui->deviceTree->clicked(child);
            return;
        }
    }
}
