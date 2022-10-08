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

#include "qkxptyprocess_win.h"
#include <windows.h>

#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>

QKxPtyProcess::QKxPtyProcess(QObject *parent)
    : QProcess(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

QKxPtyProcess::~QKxPtyProcess()
{
    if(m_bAttach) {
        ::FreeConsole();
    }
}

int QKxPtyProcess::updateSize(int cols, int rows)
{
    return -1;
}

void QKxPtyProcess::sendCtrlC()
{
    qint64 mypid = processId();
    //if(!m_bAttach) {
    //    m_bAttach = AttachConsole(mypid);
    //    qDebug() << "AttachConsole" << m_bAttach;
    //}
    if(!::GenerateConsoleCtrlEvent(CTRL_C_EVENT, mypid)) {
        char* lpMsgBuf;
        auto err = GetLastError();
        ::FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<LPTSTR>(&lpMsgBuf),
                0, nullptr );
        QString errMsg = QString::fromLocal8Bit(lpMsgBuf);
        qDebug() << "Failed to send." << "pid:" << mypid << "Reason:" << errMsg;
    }else{
        qDebug() << "Success but no effect?";
    }
}

void QKxPtyProcess::onReadyReadStandardOutput()
{
    QByteArray all = readAllStandardOutput();
    QString unicode = QString::fromLocal8Bit(all);
    QByteArray utf8 = unicode.toUtf8();
    //qDebug() << "onReadyReadStandardOutput" << unicode << utf8.length();
    emit dataArrived(utf8.data(), utf8.length());
}

void QKxPtyProcess::onReadyReadStandardError()
{
    QByteArray all = readAllStandardError();
    QString unicode = QString::fromLocal8Bit(all);
    QByteArray utf8 = unicode.toUtf8();
    //qDebug() << "onReadyReadStandardError" << unicode;
    emit dataArrived(utf8.data(), utf8.length());
}

void QKxPtyProcess::init()
{
    QObject::connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()));
    QObject::connect(this, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()));
    setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        args->flags |= CREATE_NEW_PROCESS_GROUP;
    });
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QByteArray cmdPath = qgetenv("ComSpec");
    // %SystemRoot%\system32\cmd.exe
    setProcessEnvironment(env);
    setProgram(cmdPath);
    start();
}
