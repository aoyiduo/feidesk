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

#include "qkxptyprocess_unix.h"
#include "kptydevice.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

// System
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>


QKxPtyProcess::QKxPtyProcess(QObject *parent)
    : QProcess(parent)
{
    m_pty = new KPtyDevice(this);
    m_pty->open();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

QKxPtyProcess::~QKxPtyProcess()
{

}

int QKxPtyProcess::updateSize(int cols, int rows)
{
    if (m_pty->masterFd() >= 0){
        m_pty->setWinSize(rows, cols);
    }
    return 0;
}

void QKxPtyProcess::sendCtrlC()
{
    qint64 mypid = processId();
}

void QKxPtyProcess::write(const char *pbuf, int cnt)
{
    m_pty->write(pbuf, cnt);
}

void QKxPtyProcess::setupChildProcess()
{
    m_pty->setCTty();
    dup2(m_pty->slaveFd(), 0);
    dup2(m_pty->slaveFd(), 1);
    dup2(m_pty->slaveFd(), 2);
    QProcess::setupChildProcess();
    struct sigaction action;
    sigset_t sigset;
    sigemptyset(&action.sa_mask);
    sigemptyset(&sigset);
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    for (int signal=1;signal < NSIG; signal++) {
        sigaction(signal,&action,nullptr);
        sigaddset(&sigset, signal);
    }
    sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
}

void QKxPtyProcess::onReadyRead()
{
    QByteArray data = m_pty->readAll();
    emit dataArrived(data.constData(),data.count());
}

void QKxPtyProcess::init()
{
    setUtf8Mode(true);
    setFlowControlEnabled(true);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // the follow two code is ok.
    // http://www.kxtry.com/archives/3494
    env.insert("TERM", "xterm");
    env.insert("TERM", "xterm-16color");
    // some osx system did't open the color property.
    // we follow the habit and do nothing.
#if defined(Q_OS_MAC22222)
    env.insert("CLICOLOR", "1");
    env.insert("LSCOLORS","GxFxCxDxBxegedabagaced");
#endif
    // do not use 256 color, because most of color schema is 16 color.
    // so it will cause no color show for out of local color range.
    //env.insert("TERM", "xterm-256color");
    setProcessEnvironment(env);

    QString cwd = QDir::homePath();
    setWorkingDirectory(cwd);

    QStringList programs = {QString::fromUtf8(qgetenv("SHELL")), "/bin/bash", "/bin/sh"};
    QString exec;
    for(int i = 0; i < programs.length(); i++) {
        exec = QStandardPaths::findExecutable(programs.at(i));
        if(!exec.isEmpty()) {
            break;
        }
    }
    setProgram(exec);

    struct ::termios ttmode;
    m_pty->tcGetAttr(&ttmode);
    if (!_xonXoff){
        ttmode.c_iflag &= ~(IXOFF | IXON);
    } else {
        ttmode.c_iflag |= (IXOFF | IXON);
    }
#ifdef IUTF8 // XXX not a reasonable place to check it.
    if (!_utf8){
        ttmode.c_iflag &= ~IUTF8;
    } else {
        ttmode.c_iflag |= IUTF8;
    }
#endif
    bool _eraseChar = 0;
    if (_eraseChar != 0){
        ttmode.c_cc[VERASE] = _eraseChar;
    }
    if (!m_pty->tcSetAttr(&ttmode)){
        qWarning() << "Unable to set terminal attributes.";
    }
    m_pty->setWinSize(40, 80);

    QObject::connect(m_pty, SIGNAL(readyRead()) , this , SLOT(onReadyRead()));
    start();
}

void QKxPtyProcess::setUtf8Mode(bool enable)
{
#ifdef IUTF8 // XXX not a reasonable place to check it.
  _utf8 = enable;

  if (m_pty->masterFd() >= 0)
  {
    struct ::termios ttmode;
    m_pty->tcGetAttr(&ttmode);
    if (!enable)
      ttmode.c_iflag &= ~IUTF8;
    else
      ttmode.c_iflag |= IUTF8;
    if (!m_pty->tcSetAttr(&ttmode))
      qWarning() << "Unable to set terminal attributes.";
  }
#endif
}

void QKxPtyProcess::setFlowControlEnabled(bool enable)
{
  _xonXoff = enable;

  if (m_pty->masterFd() >= 0)
  {
    struct ::termios ttmode;
    m_pty->tcGetAttr(&ttmode);
    if (!enable)
      ttmode.c_iflag &= ~(IXOFF | IXON);
    else
      ttmode.c_iflag |= (IXOFF | IXON);
    if (!m_pty->tcSetAttr(&ttmode))
      qWarning() << "Unable to set terminal attributes.";
  }
}
