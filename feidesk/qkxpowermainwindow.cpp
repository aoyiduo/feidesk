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

#include "qkxpowermainwindow.h"
#include "ui_qkxpowermainwindow.h"
#include "qkxiconlabelbutton.h"
#include "qkxoptionform.h"
#include "qkxmydeviceform.h"
#include "qkxconnectionform.h"
#include "qkxsettingex.h"
#include "qkxpowerapplication.h"
#include "qkxnetclient.h"
#include "qkxdeskdef.h"
#include "qkxutils.h"
#include "qkxserverdialog.h"
#include "qkxlanguagedialog.h"
#include "qkxaboutdialog.h"
#include "qkxoptionsdialog.h"

#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QLabel>
#include <QButtonGroup>

#include <qtservice.h>

QKxPowerMainWindow::QKxPowerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QKxPowerMainWindow)
    , m_hasLogin(false)
    , m_doAutoLogin(false)
    , m_deviceId(0)
    , m_netCheckCount(0)
    , m_bCancelAutoHide(false)
{
    ui->setupUi(this);
    setObjectName("mainWindow");
    setWindowTitle("FeiDesk");

    ui->brandWidget->installEventFilter(this);
    ui->brandWidget->setCursor(Qt::PointingHandCursor);
    ui->controlPanel->setFixedWidth(220);

    initControlMenu();
    initMenuBar();
    initStatusBar();

    m_myDevice = new QKxMyDeviceForm(this);
    m_connection = new QKxConnectionForm(this);
    ui->stackPanel->addWidget(m_myDevice);
    ui->stackPanel->addWidget(m_connection);
    ui->stackPanel->setCurrentWidget(m_connection);

    setMaximumSize(1024, 768);
    resize(800, 600);
    restoreLastState();

    QObject::connect(m_myDevice, SIGNAL(loginResult(qint32,qint32,QString)), this, SLOT(onAccountLoginResult(qint32,qint32,QString)));
    QObject::connect(m_myDevice, SIGNAL(loginResult(qint32,qint32,QString)), m_connection, SLOT(onAccountLoginResult(qint32,qint32,QString)));
    QObject::connect(m_myDevice, SIGNAL(logoutArrived()), m_connection, SLOT(onLogoutArrived()));

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onNetworkStatusChecking()));
    timer->start(2000);
    m_netCheck = timer;

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "onNetworkStatusChecking", Qt::QueuedConnection);

    QPointer<QKxPowerMainWindow> that=this;
    QTimer::singleShot(3000, this, [=](){        
        if(that != nullptr && !that->m_bCancelAutoHide) {
            that->close();
        }
    });
}

QKxPowerMainWindow::~QKxPowerMainWindow()
{
    delete ui;
}

void QKxPowerMainWindow::cancelAutoHide()
{
    m_bCancelAutoHide = true;
}

QKxMyDeviceForm *QKxPowerMainWindow::myDeviceForm()
{
    return QKxPowerApplication::mainWindow()->m_myDevice;
}

QKxConnectionForm *QKxPowerMainWindow::connectionForm()
{
    return QKxPowerApplication::mainWindow()->m_connection;
}

void QKxPowerMainWindow::onAccountLoginResult(qint32 err, qint32 okcnt, const QString &msg)
{
    ui->actionExitAccount->setVisible(err == 0);
    m_hasLogin = err == 0;
    if(err == 0) {
        m_btnMyDevice->setChecked(true);
        onButtonClicked(m_btnMyDevice);
    }
}

void QKxPowerMainWindow::onAccountLogout()
{
    ui->actionExitAccount->setVisible(false);
    m_hasLogin = false;
    m_myDevice->logout();    
    m_btnMyDevice->setChecked(true);
    onButtonClicked(m_btnMyDevice);
}

void QKxPowerMainWindow::onAnonymousConnect()
{

}

void QKxPowerMainWindow::onNetworkStatusChecking()
{
    QKxPowerApplication *app = qobject_cast<QKxPowerApplication*>(QKxPowerApplication::instance());
    QKxNetClient *net = app->netClient();
    m_netCheckCount++;
    int cnt = m_netCheckCount % 10;
    int dot = m_netCheckCount % 4;
    QString tip;
    for(int i = 0; i < dot; i++) {
        tip += ".";
    }
    tip += QString("[%1s]").arg(cnt);

    syncNetInfo();
    if(!net->isIPCConnected()) {
        tip = tr("Try to establish communication") + tip;
    }else if(!net->isServerConfigureInitialized()) {
        tip = tr("Try to initialize service") + tip;
    }else if(!net->isMACCheckFinished()){
        tip = tr("Connecting to server") + tip;
    }else if(!net->isP2PCheckFinished()){
        tip = tr("P2P network type check") + tip;
    }else if(net->isNetworkReconnecting()) {
        tip = tr("Try to reconnect to server") + tip;
    }else{
        tip = tr("Server connection completed");
        m_netCheckCount = 0;
        if(m_deviceId > 0) {
            m_netCheck->stop();
        }
        if(QKxSettingEx::autoLogin() && !m_doAutoLogin) {
            m_doAutoLogin = true;
            QMetaObject::invokeMethod(m_myDevice, "onAutoStartLogin", Qt::QueuedConnection);
        }
    }
    m_netStatus->setText(tip);
}

void QKxPowerMainWindow::onIpcConnectedArrived()
{
    QMetaObject::invokeMethod(m_connection, "onDevicePasswordAction", Qt::QueuedConnection, Q_ARG(int, 0));
}

void QKxPowerMainWindow::onMacCheckFinishedArrived()
{

}

void QKxPowerMainWindow::onServerConfigureInitializedArrived(bool ok, const QString &msg)
{

}

void QKxPowerMainWindow::onP2pCheckFinishedArrived()
{

}

void QKxPowerMainWindow::onNetworkReconnecting()
{
    m_netCheck->start(2000);
}

void QKxPowerMainWindow::onNetStateArrived(int state, const QString &msg)
{

}

void QKxPowerMainWindow::onMenuHelpAboutOpen()
{
    QKxAboutDialog dlg(this);
    dlg.exec();
}

void QKxPowerMainWindow::onMenuHelpVersionCheck()
{
    QDesktopServices::openUrl(QUrl("http://www.aoyiduo.com/feidesk"));
}

void QKxPowerMainWindow::onMenuWebsiteOpen()
{
    QDesktopServices::openUrl(QUrl("http://www.aoyiduo.com"));
}

void QKxPowerMainWindow::onMenuManageDevice()
{
    QString url = QKxSettingEx::deviceManageWebsite();
    QDesktopServices::openUrl(QUrl(url));
}

void QKxPowerMainWindow::onMenuServerConfigure()
{
    QKxServerDialog dlg(this);
    if(dlg.exec() == QDialog::Accepted) {
        if(QMessageBox::information(this, tr("Restart Confirm"), tr("It's try to restart application now?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
            QKxPowerApplication::restart();
        }
    }
}

void QKxPowerMainWindow::onMenuLanguageConfigure()
{
    QKxLanguageDialog dlg(this);
    if(dlg.exec() == QDialog::Accepted) {
        if(QMessageBox::information(this, tr("Restart Confirm"), tr("It's try to restart application now?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
            QKxPowerApplication::restart();
        }
    }
}

void QKxPowerMainWindow::onMenuOptionsConfigure()
{
    QKxOptionsDialog dlg(this);
    dlg.exec();
    QFlags<QKxOptionsDialog::EChangeFlag> flags = dlg.result();
    if(flags.testFlag(QKxOptionsDialog::ECF_Connection)) {
        QMessageBox::information(this, tr("Restart Confirm"), tr("the network connection has been changed, the network will be restart"));
        QKxPowerApplication::instance()->closeAllRemoteProcess();
        QKxPowerApplication::netClient()->resetNetworkConnection();
        QKxPowerApplication::netClient()->resetP2PProirity();
    }else if(flags.testFlag(QKxOptionsDialog::ECF_P2PMode)){
        QKxPowerApplication::netClient()->resetP2PProirity();
    }
}

void QKxPowerMainWindow::onMenuAccountExit()
{
    onAccountLogout();
}

void QKxPowerMainWindow::onMenuInstallService()
{
    qDebug() << "onMenuRegisterService";
    QString path = QCoreApplication::applicationFilePath();
    qInfo() << "install capture service module" << path;
    if(QKxUtils::shellExecuteAsAdmin(QDir::toNativeSeparators(path), QStringList() << "-install", true)) {
        QMetaObject::invokeMethod(this, "updateServiceStatus", Qt::QueuedConnection);
    }
}

void QKxPowerMainWindow::onMenuUninstallService()
{
    qDebug() << "onMenuRemoveService";
    QtServiceController ctrl(SERVICE_NAME);
    if(!ctrl.isInstalled()) {
        return;
    }
    QString path = QCoreApplication::applicationFilePath();
    qInfo() << "uninstall capture service module" << path;
    if(QKxUtils::shellExecuteAsAdmin(QDir::toNativeSeparators(path), QStringList() << "-uninstall", true)) {
        QMetaObject::invokeMethod(this, "updateServiceStatus", Qt::QueuedConnection);
    }
}

void QKxPowerMainWindow::onMenuCleanAccount()
{
    onAccountLogout();
    QKxSettingEx::cleanAutoLogin();
    m_myDevice->cleanAccount();
}

void QKxPowerMainWindow::onButtonClicked(QAbstractButton *btn)
{
    QString name = btn->objectName();
    if(name == "connection") {
        ui->stackPanel->setCurrentWidget(m_connection);
    }else if(name == "device") {
        ui->stackPanel->setCurrentWidget(m_myDevice);
    }/*else if(name == "option") {
        ui->stackPanel->setCurrentWidget(m_option);
    }*/
}

void QKxPowerMainWindow::closeEvent(QCloseEvent *e)
{
    QDialog *blockChild = findChild<QDialog*>();
    if(blockChild) {
        if(blockChild->close()){
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        }
        e->setAccepted(false);
        return;
    }
    saveLastState();
    e->setAccepted(false);
    setVisible(false);
}

void QKxPowerMainWindow::init()
{
    syncNetInfo();
}

void QKxPowerMainWindow::updateServiceStatus()
{
    ui->actionExitAccount->setVisible(m_hasLogin);
}

bool QKxPowerMainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QEvent::Type t = event->type();
    if(t == QEvent::MouseButtonPress) {
        QWidget *w = qobject_cast<QWidget*>(watched);
        if(w == ui->brandWidget) {
            QDesktopServices::openUrl(QUrl("http://www.aoyiduo.com"));
            return true;
        }
    }
    return false;
}

void QKxPowerMainWindow::initControlMenu()
{
    QButtonGroup *group = new QButtonGroup(this);
    QKxIconLabelButton *btnConnection = new QKxIconLabelButton(QIcon(QPixmap(":/resource/skin/list_connect.png").scaled(36, 36, Qt::KeepAspectRatio ,Qt::SmoothTransformation)),
                                                       tr("Connection"), this);
    QFont ft = font();
    ft.setPointSize(15);
    btnConnection->setFixedHeight(48);
    btnConnection->setFont(ft);
    btnConnection->setIconSize(QSize(36,36));
    btnConnection->setContentsMargins(10, 5, 10, 5);
    btnConnection->setBackgroundColor(QColor(Qt::white));
    btnConnection->setChecked(true);
    btnConnection->setObjectName("connection");
    ui->optionArea->addWidget(btnConnection);
    QKxIconLabelButton *btnDevice = new QKxIconLabelButton(QIcon(QPixmap(":/resource/skin/list_account.png").scaled(36, 36, Qt::KeepAspectRatio ,Qt::SmoothTransformation)),
                                                   tr("Device"), this);
    btnDevice->setFixedHeight(48);
    btnDevice->setFont(ft);
    btnDevice->setIconSize(QSize(36,36));
    btnDevice->setContentsMargins(10, 5, 10, 5);
    btnDevice->setBackgroundColor(QColor(Qt::white));
    btnDevice->setObjectName("device");
    ui->optionArea->addWidget(btnDevice);
    m_btnMyDevice = btnDevice;

#if 0
    QKxIconLabelButton *btnOption = new QKxIconLabelButton(QIcon(QPixmap(":/resource/skin/list_option.png").scaled(36, 36, Qt::KeepAspectRatio ,Qt::SmoothTransformation)),
                                                    tr("Option"), this);

    btnOption->setFixedHeight(48);
    btnOption->setFont(ft);
    btnOption->setIconSize(QSize(36,36));
    btnOption->setContentsMargins(10, 5, 10, 5);
    btnOption->setBackgroundColor(QColor(Qt::white));
    btnOption->setObjectName("option");
    ui->optionArea->addWidget(btnOption);
#endif
    group->addButton(btnConnection);
    group->addButton(btnDevice);
    //group->addButton(btnOption);
    QObject::connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

bool QKxPowerMainWindow::restoreLastState()
{
    bool has = false;
    QByteArray geom = QKxSettingEx::value("mainwindow/geometry", "").toByteArray();
    if(!geom.isEmpty()) {
        restoreGeometry(geom);
        has = true;
    }

    QByteArray buf = QKxSettingEx::value("mainwindow/lastLayout", "").toByteArray();
    if(!buf.isEmpty()) {
        restoreState(buf);
        has = true;
    }
    return has;
}

void QKxPowerMainWindow::saveLastState()
{
    QByteArray state = saveState();
    QKxSettingEx::setValue("mainwindow/lastLayout", state);
    QByteArray geom = saveGeometry();
    QKxSettingEx::setValue("mainwindow/geometry", geom);
}

void QKxPowerMainWindow::syncNetInfo()
{
    if(m_deviceId > 0) {
        return;
    }
    QKxPowerApplication *app = qobject_cast<QKxPowerApplication*>(QKxPowerApplication::instance());
    QKxNetClient *net = app->netClient();
    qint64 did = net->deviceID();
    if(did > 0) {
        m_deviceId = did;
        m_connection->setDeviceID(did);
    }
}

void QKxPowerMainWindow::initMenuBar()
{
    QMenuBar *bar = ui->menubar;
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(onMenuHelpAboutOpen()));
    QObject::connect(ui->actionVersion, SIGNAL(triggered(bool)), this, SLOT(onMenuHelpVersionCheck()));
    QObject::connect(ui->actionWebsite, SIGNAL(triggered(bool)), this, SLOT(onMenuWebsiteOpen()));
    QObject::connect(ui->actionManageDevice, SIGNAL(triggered(bool)), this, SLOT(onMenuManageDevice()));
    QObject::connect(ui->actionExitAccount, SIGNAL(triggered(bool)), this, SLOT(onMenuAccountExit()));
    QObject::connect(ui->actionServer, SIGNAL(triggered(bool)), this, SLOT(onMenuServerConfigure()));
    QObject::connect(ui->actionLanguage, SIGNAL(triggered(bool)), this, SLOT(onMenuLanguageConfigure()));
    QObject::connect(ui->actionOptions, SIGNAL(triggered(bool)), this, SLOT(onMenuOptionsConfigure()));
    QObject::connect(ui->actionCleanAccount, SIGNAL(triggered(bool)), this, SLOT(onMenuCleanAccount()));
    updateServiceStatus();
}

void QKxPowerMainWindow::initStatusBar()
{
    QStatusBar *bar = ui->statusBar;
    {
        bar->addWidget(new QLabel(tr("Network Status:"), bar));
        m_netStatus = new QLabel("network checking", bar);
        bar->addWidget(m_netStatus);
        //QFrame *vline = new QFrame(bar);
        //vline->setFrameShape(QFrame::VLine);
        //bar->addWidget(vline);
    }
}
