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

#include <QApplication>
#include <QDebug>
#include <QStyleFactory>

#include "qkxpowerapplication.h"
#include "qkxpowernetserver.h"
#include "qkxsettingex.h"
#include "qkxserviceapplication.h"
#include "qkxutils.h"
#include "qkxdeskdef.h"
#include "qkxfirewall.h"
#include "qkxdaemonslave.h"
#include "qkxtcpvncserver.h"
#include "qkxlocalvncserver.h"
#include "qkxvncserverpeer.h"
#include "qkxnetserver.h"
#include "qkxnetclient.h"
#include "qkxdesktopplaywindow.h"
#include "qkxfiletransferwindow.h"
#include "qkxlocalpeer.h"
#include "qkxsharememory.h"
#include "qkxftpserver.h"
#include "qkxrc4crypt.h"
#include "qkxxorcrypt.h"
#include "qkxservicemanager.h"
#include "qkxcapserver.h"
#include "qkxcapoption.h"
#include "qkxwallpaper.h"
#include "qkxtermwindow.h"
#include "qkxchatwindow.h"
#include "qkxkeepwakeup.h"

#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QTranslator>

static QFile g_fileLog;
static QMutex g_mutexFileLog;

#ifdef Q_OS_WIN
#define WITH_SID        (false)
#else
#define WITH_SID        (true)
#endif

static void MyMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & text)
{
    const QDateTime datetime = QDateTime::currentDateTime();
    const char * typeText = NULL;
    switch (type)
    {
    case QtDebugMsg:
    case QtInfoMsg:
        typeText = "Info";
        break;
    case QtWarningMsg:
        typeText = "Warning";
        break;
    case QtCriticalMsg:
        typeText = "Critical";
        break;
    case QtFatalMsg:
        typeText = "Fatal";
        break;
    }
    const QString finalText = QString("%1 %2 %3\n").arg(datetime.toString("yyyyMMdd/hh:mm:ss.zzz")).arg(typeText).arg(text);
    if (g_fileLog.isOpen())
    {
        QMutexLocker locker(&g_mutexFileLog);
        if (g_fileLog.size() == 0)
            g_fileLog.write("\xef\xbb\xbf");
        g_fileLog.write(finalText.toUtf8());
        g_fileLog.flush();
    }
}

void setDebugMessageToFile(const QString& name, bool tryDelete = false)
{
    QString path = QKxSettingEx::applicationDataPath();
    QString fullFile = path + "/" + name;
    qputenv("APP_LOG_PATH", fullFile.toUtf8());
    QFileInfo fi(fullFile);
    int fs = fi.size();
    if(fs > 1024 * 1024 || tryDelete) {
        QFile::remove(fullFile);
    }
    qInstallMessageHandler(MyMessageHandler);
    g_fileLog.setFileName(fullFile);
    g_fileLog.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

int InstallMain(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    setDebugMessageToFile("install.log", true);

    return QKxCapServer::InstallMain(argc, argv, SERVICE_NAME);
}

int UninstallMain(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    setDebugMessageToFile("uninstall.log", true);
    return QKxCapServer::UninstallMain(argc, argv, SERVICE_NAME);
}

void VncMainCallBack(QGuiApplication *app)
{
    setDebugMessageToFile("vnc.log");
    qDebug() << "VncMain" << QKxSettingEx::applicationDataPath();
    QKxSettingEx::updateStartTime();
}

int VncMain(int argc, char *argv[])
{
    return QKxCapServer::VncMain(argc, argv, VncMainCallBack);
}

int DeskMain(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QByteArray did = qgetenv("REMOTE_DEVICE_ID");
    if(did.isEmpty()) {
        return 0;
    }
    QKxLocalPeer desk("desk:"+did, WITH_SID);
    if(desk.isClient()) {
        desk.sendMessage("active");
        return 0;
    }
    QString path = QKxSettingEx::applicationDataPath();
    qDebug() << "program path" << app.applicationFilePath();
    QStringList styles = QStyleFactory::keys();
    qDebug() << "embeded style list: " << styles;
    QApplication::setStyle("Fusion");
    QFile f(":/resource/qss/default.qss");
    f.open(QFile::ReadOnly);
    QByteArray qss = f.readAll();
    f.close();
    app.setStyleSheet(qss);

    QTranslator translator;
    QString lang = QKxSettingEx::languageFile();
     if(!lang.isEmpty() && translator.load(lang)){
        app.installTranslator(&translator);
    }


    QKxDesktopPlayWindow *main = new QKxDesktopPlayWindow(nullptr);
    QObject::connect(&app, SIGNAL(aboutToQuit()), main, SLOT(deleteLater()));
    QObject::connect(&desk, SIGNAL(messageReceived(QString)), main, SLOT(onMessageReceived(QString)));
    main->resize(1024, 768);
    main->show();
    return app.exec();
}

int TermMain(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QByteArray did = qgetenv("REMOTE_DEVICE_ID");
    if(did.isEmpty()) {
        return 0;
    }

    QString path = QKxSettingEx::applicationDataPath();
    qDebug() << "program path" << app.applicationFilePath();
    QStringList styles = QStyleFactory::keys();
    qDebug() << "embeded style list: " << styles;
    QApplication::setStyle("Fusion");
    QFile f(":/resource/qss/default.qss");
    f.open(QFile::ReadOnly);
    QByteArray qss = f.readAll();
    f.close();
    app.setStyleSheet(qss);

    QTranslator translator;
    QString lang = QKxSettingEx::languageFile();
     if(!lang.isEmpty() && translator.load(lang)){
        app.installTranslator(&translator);
    }

    QKxTermWindow *main = new QKxTermWindow(nullptr);
    QObject::connect(&app, SIGNAL(aboutToQuit()), main, SLOT(deleteLater()));
    main->resize(800, 640);
    main->show();

    return app.exec();
}

int ChatMain(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QByteArray did = qgetenv("REMOTE_DEVICE_ID");
    if(did.isEmpty()) {
        return 0;
    }

    QKxLocalPeer chat("chat:"+did, WITH_SID);
    if(chat.isClient()) {
        chat.sendMessage("active");
        return 0;
    }

    QString path = QKxSettingEx::applicationDataPath();
    qDebug() << "program path" << app.applicationFilePath();
    QStringList styles = QStyleFactory::keys();
    qDebug() << "embeded style list: " << styles;
    QApplication::setStyle("Fusion");
    QFile f(":/resource/qss/default.qss");
    f.open(QFile::ReadOnly);
    QByteArray qss = f.readAll();
    f.close();
    app.setStyleSheet(qss);

    QTranslator translator;
    QString lang = QKxSettingEx::languageFile();
    if(!lang.isEmpty() && translator.load(lang)){
        app.installTranslator(&translator);
    }

    QKxChatWindow *main = new QKxChatWindow(nullptr);
    QObject::connect(&app, SIGNAL(aboutToQuit()), main, SLOT(deleteLater()));
    QObject::connect(&chat, SIGNAL(messageReceived(QString)), main, SLOT(onMessageReceived(QString)));
    main->resize(320, 480);
    main->show();

    return app.exec();
}

int FtpMain(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QByteArray did = qgetenv("REMOTE_DEVICE_ID");
    if(did.isEmpty()) {
        return 0;
    }
    QKxLocalPeer ftp("ftp:"+did, WITH_SID);
    if(ftp.isClient()) {
        ftp.sendMessage("active");
        return 0;
    }
    QString path = QKxSettingEx::applicationDataPath();
    qDebug() << "program path" << app.applicationFilePath();
    QStringList styles = QStyleFactory::keys();
    qDebug() << "embeded style list: " << styles;
    QApplication::setStyle("Fusion");
    QFile f(":/resource/qss/default.qss");
    f.open(QFile::ReadOnly);
    QByteArray qss = f.readAll();
    f.close();
    app.setStyleSheet(qss);

    QTranslator translator;
    QString lang = QKxSettingEx::languageFile();
     if(!lang.isEmpty() && translator.load(lang)){
        app.installTranslator(&translator);
    }

    QKxFileTransferWindow *main = new QKxFileTransferWindow(nullptr);
    QObject::connect(&app, SIGNAL(aboutToQuit()), main, SLOT(deleteLater()));
    QObject::connect(&ftp, SIGNAL(messageReceived(QString)), main, SLOT(onMessageReceived(QString)));
    main->resize(800, 640);
    main->show();

    return app.exec();
}

int PowerMainRestart(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    // remove current process environment variables to avoid infinte recursive startup.
    qputenv("RUN_ACTION_NAME", "");
    QString path = app.applicationFilePath();
    QTimer timer;
    int trycnt = 0;
    bool exist = true;
    bool ok = QObject::connect(&timer, &QTimer::timeout, [&](){
        if(exist) {
            QKxLocalPeer power("power", WITH_SID, &app);
            exist = power.isClient();
        }else{
            QProcess::startDetached(path);
            timer.stop();
            app.quit();
        }
        qDebug() << "tryRestartPowerMain" << trycnt++;
    });
    timer.start(1000);
    return app.exec();
}

int PowerMain(int argc, char *argv[]) {
    QKxPowerApplication app(argc, argv);
    QKxLocalPeer power("power", WITH_SID, &app);
    if(power.isClient()) {
        power.sendMessage("active");
        return 0;
    }

    if(!QKxUtils::isDebugBuild()) {
        setDebugMessageToFile("power.log");
    }

    QObject::connect(&power, SIGNAL(messageReceived(QString)), &app, SLOT(onMessageReceived(QString)));

    QString path = QKxSettingEx::applicationDataPath();
    QString addr = QKxSettingEx::deviceLoginAddress();
    qDebug() << "program path" << addr << QCoreApplication::applicationFilePath();

    int mode = QKxSettingEx::value("network/connection", 1).toInt();
    qputenv("NETHOST_CONNECTION_MODE", QByteArray::number(mode));
    QString nodePath = QKxCapOption::instance()->serverUrlNodePath();
    QKxNetServer server("NETHOST_PROCESS_URL", addr, path, SERVICE_NAME, nodePath, &app);
    qputenv("NETHOST_PROCESS_PID", QByteArray::number(app.applicationPid()));


    QStringList styles = QStyleFactory::keys();
    qDebug() << "embeded style list: " << styles;
    QApplication::setStyle("Fusion");
    QFile f(":/resource/qss/default.qss");
    f.open(QFile::ReadOnly);
    QByteArray qss = f.readAll();
    f.close();
    app.setStyleSheet(qss);
    QApplication::setWindowIcon(QIcon(":/resource/skin/feidesk2.png"));
    QKxCapServer::ExitWithParentProcess(&app);
    QMetaObject::invokeMethod(&app, "onAppStart", Qt::QueuedConnection);    

    QTranslator translator;
    QString lang = QKxSettingEx::languageFile();
     if(!lang.isEmpty() && translator.load(lang)){
        app.installTranslator(&translator);
    }

    QKxKeepWakeUp wakeup;

    return app.exec();
}

#ifdef Q_OS_WIN
int ActiveMain(int argc, char *argv[])
{
    QKxLocalPeer power("power", WITH_SID);
    if(power.isClient()) {
        power.sendMessage("active");
        return 0;
    }

    QString path = QKxUtils::applicationFilePath();
    QKxUtils::checkFirewall();
    QtServiceController sc(SERVICE_NAME);
    if(sc.isInstalled()) {
        if(sc.isSameService(path)) {
            if(sc.isRunning()) {
                //sc.sendCommand(SERVICE_COMMAND_CODE_START);
            }else if(QKxUtils::isRunAsAdmin()){
                sc.start();
            }else{
                QKxUtils::shellExecuteAsAdmin(QDir::toNativeSeparators(path), QStringList() << "-start", true);
            }
        }else{
            QKxUtils::checkFirewall();
            QKxUtils::shellExecuteAsAdmin(QDir::toNativeSeparators(path), QStringList() << "-install", true);
        }
    }else{
        QKxUtils::checkFirewall();
        QKxUtils::shellExecuteAsAdmin(QDir::toNativeSeparators(path), QStringList() << "-install", true);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QStringList args = QKxServiceApplication::internalArguments();
    args << "-service";
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-install") == 0) {
            return InstallMain(argc, argv);
        }else if(strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "-uninstall") == 0) {
            return UninstallMain(argc, argv);
        }else if(args.contains(argv[i])) {
            QKxServiceApplication service(argc, argv, SERVICE_NAME);
            return service.exec();
        }
    }
    QKxSettingEx::updateStartTime();
    QByteArray name = qgetenv("RUN_ACTION_NAME");
    if(name.startsWith("restart:")) {
        return PowerMainRestart(argc, argv);
    }else if(name.startsWith("main:")) {
        return PowerMain(argc, argv);
    }else if(name.startsWith("vnc:")) {
        return VncMain(argc, argv);
    }else if(name.startsWith("desk:")) {
        return DeskMain(argc, argv);
    }else if(name.startsWith("term:")) {
        return TermMain(argc, argv);
    }else if(name.startsWith("chat:")) {
        return ChatMain(argc, argv);
    }else if(name.startsWith("ftp:")) {
        return FtpMain(argc, argv);
    }
    return ActiveMain(argc, argv);
}
#else
int main(int argc, char *argv[])
{
    QKxSettingEx::updateStartTime();
    QKxUtils::disableAppNap();
    QByteArray name = qgetenv("RUN_ACTION_NAME");
    if(name.startsWith("restart:")) {
        return PowerMainRestart(argc, argv);
    }else if(name.startsWith("vnc:")) {
        return VncMain(argc, argv);
    }else if(name.startsWith("desk:")) {
        return DeskMain(argc, argv);
    }else if(name.startsWith("term:")) {
        return TermMain(argc, argv);
    }else if(name.startsWith("chat:")) {
        return ChatMain(argc, argv);
    }else if(name.startsWith("ftp:")) {
        return FtpMain(argc, argv);
    }
    return PowerMain(argc, argv);
}
#endif
