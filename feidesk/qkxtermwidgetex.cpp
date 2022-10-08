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

#include "qkxtermwidgetex.h"
#include "qkxtermitem.h"
#include "qkxsettingex.h"
#include "qkxutils.h"
#include "qkxptyrequest.h"
#include "qkxechoinput.h"

#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>

QKxTermWidgetEx::QKxTermWidgetEx(QWidget *parent)
    : QKxTermWidget(parent)
{
    static int idx = 0;
    setObjectName(QString("QKxTermWidgetEx:%1").arg(idx++));
    addToTermImpl();
    setAttribute(Qt::WA_DeleteOnClose);
    initDefault();
    initCustom();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    QString val = QKxSettingEx::value("property/shortcut", "").toString();
    QVariantMap mdata = QKxUtils::qBase64ToVariant(val).toMap();
    QKxTermItem *term = termItem();
    term->bindShortCut(QKxTermItem::SCK_Copy, mdata.value("SCK_Copy", m_term->defaultShortCutKey(QKxTermItem::SCK_Copy)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_Paste, mdata.value("SCK_Paste", m_term->defaultShortCutKey(QKxTermItem::SCK_Paste)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectAll, mdata.value("SCK_SelectAll", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectAll)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectLeft, mdata.value("SCK_SelectLeft", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectLeft)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectRight, mdata.value("SCK_SelectRight", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectRight)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectUp, mdata.value("SCK_SelectUp", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectUp)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectDown, mdata.value("SCK_SelectDown", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectDown)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectHome, mdata.value("SCK_SelectHome", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectHome)).value<QKeySequence>());
    term->bindShortCut(QKxTermItem::SCK_SelectEnd, mdata.value("SCK_SelectEnd", m_term->defaultShortCutKey(QKxTermItem::SCK_SelectEnd)).value<QKeySequence>());


    m_input = new QKxEchoInput(m_term, this);
    QObject::connect(m_input, SIGNAL(commandArrived(QByteArray)), term, SIGNAL(sendData(QByteArray)));
    QObject::connect(m_input, SIGNAL(ctrlCArrived()), this, SLOT(onTermCtrlCArrived()));
    QObject::connect(term, SIGNAL(termSizeChanged(int,int)), m_input, SLOT(onTermSize(int,int)));

    initDefault();

    term->setEchoInput(m_input);
    term->setEchoInputEnabled(true);
    term->setEchoInputEnabled(false);
    QObject::connect(term, SIGNAL(termSizeChanged(int,int)), this, SLOT(onTermSizeChanged(int,int)));
    QObject::connect(term, SIGNAL(sendData(QByteArray)), this, SLOT(onTermSendData(QByteArray)));
}

void QKxTermWidgetEx::initDefault()
{
    QString val = QKxSettingEx::value("property/default", "").toString();
    QVariantMap mdata = QKxUtils::qBase64ToVariant(val).toMap();
    resetProperty(mdata);
}

void QKxTermWidgetEx::initCustom()
{
}

void QKxTermWidgetEx::addToTermImpl()
{

}

void QKxTermWidgetEx::resetProperty(QVariantMap mdata)
{
    if(mdata.isEmpty()) {
        return;
    }
    QString schema = mdata.value("colorSchema", DEFAULT_COLOR_SCHEMA).toString();
    m_term->setColorSchema(schema);

    QString keyboard = mdata.value("keyboard", DEFAULT_KEY_LAYOUT).toString();
    m_term->setKeyLayoutByName(keyboard);

    QString codec = mdata.value("textcodec", DEFAULT_TEXT_CODEC).toString();
    m_term->setTextCodec(codec);

    QString fontName = mdata.value("fontName", DEFAULT_FONT_FAMILY).toString();
    int fontSize = mdata.value("fontSize", DEFAULT_FONT_SIZE).toInt();
    QFont ft = m_term->font();
    ft.setFamily(fontName);
    ft.setPointSize(fontSize);
    m_term->setFont(ft);

    QString cursorType = mdata.value("cursorType", "block").toString();
    if(cursorType.isEmpty() || cursorType == "block") {
        m_term->setCursorType(QKxTermItem::CT_Block);
    }else if(cursorType == "underline") {
        m_term->setCursorType(QKxTermItem::CT_Underline);
    }else {
        m_term->setCursorType(QKxTermItem::CT_IBeam);
    }
    int lines = mdata.value("historyLength", 1200).toInt();
    m_term->setHistorySize(lines);
}

void QKxTermWidgetEx::onPtyDataArrived(const QByteArray &buf)
{
    m_term->parse(buf);
}

void QKxTermWidgetEx::onTermCtrlCArrived()
{
    if(m_pty) {
        m_pty->sendCtrlC();
    }
}

void QKxTermWidgetEx::onTermSizeChanged(int lines, int columns)
{
    if(m_pty){
        m_pty->updateSize(columns, lines);
    }
}

void QKxTermWidgetEx::onTermSendData(const QByteArray &buf)
{
    if(m_pty) {
        m_term->scrollToEnd();
        m_pty->sendData(buf);
    }
}

void QKxTermWidgetEx::onConnected()
{
    if(m_pty) {
        QSize sz = m_term->termSize();
        m_pty->updateSize(sz.width(), sz.height());
    }
}

void QKxTermWidgetEx::onDisconnected()
{

}

void QKxTermWidgetEx::onPlatformTypeArrived(int platform, const QByteArray &os)
{
    if(platform == 1) {
        // windows
        m_term->setEchoInputEnabled(true);
    }else if(platform == 2) {
        // mac
        m_term->setEchoInputEnabled(false);
    }else if(platform == 3) {
        // unix
        m_term->setEchoInputEnabled(false);
    }else if(platform == 4) {
        // android
        m_term->setEchoInputEnabled(false);
    }
}

void QKxTermWidgetEx::onCopyToClipboard()
{
    m_term->tryToCopy();
}

void QKxTermWidgetEx::onPasteFromClipboard()
{
    m_term->tryToPaste();
}

void QKxTermWidgetEx::onShowFindBar()
{
    setFindBarVisible(true);
}

void QKxTermWidgetEx::onPeerConnectedArrived(const QString &addr)
{
    m_serverAddr = addr;
    reconnect();
}

void QKxTermWidgetEx::contextMenuEvent(QContextMenuEvent *ev)
{
    QMenu menu(this);
    QAction *copy = menu.addAction(QIcon(":/woterm/resource/skin/copy.png"), tr("Copy"), this, SLOT(onCopyToClipboard())/*, QKeySequence(Qt::ALT +  Qt::Key_C)*/);
    QAction *paste = menu.addAction(QIcon(":/woterm/resource/skin/paste.png"), tr("Paste"), this, SLOT(onPasteFromClipboard())/*, QKeySequence(Qt::ALT +  Qt::Key_P)*/);
    QAction *find = menu.addAction(QIcon(":/woterm/resource/skin/find.png"), tr("Find..."), this, SLOT(onShowFindBar())/*, QKeySequence(Qt::CTRL +  Qt::Key_F)*/);

    QString selTxt = m_term->selectedText();
    copy->setDisabled(selTxt.isEmpty());
    QClipboard *clip = QGuiApplication::clipboard();
    QString clipTxt = clip->text();
    paste->setDisabled(clipTxt.isEmpty());
    menu.exec(QCursor::pos());
}

void QKxTermWidgetEx::reconnect()
{
    if(m_pty) {
        m_pty->deleteLater();
    }
    m_pty = new QKxPtyRequest(this);
    QObject::connect(m_pty, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_pty, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    QObject::connect(m_pty, SIGNAL(dataArrived(QByteArray)), this, SLOT(onPtyDataArrived(QByteArray)));
    QObject::connect(m_pty, SIGNAL(platformTypeArrived(int,QByteArray)), this, SLOT(onPlatformTypeArrived(int,QByteArray)));
    QStringList hps = m_serverAddr.split(':');
    if(hps.length() < 3) {
        return;
    }
    QString host = hps.at(1);
    if(host.startsWith("//")) {
        host = host.mid(2);
    }
    QString port = hps.at(2);
    m_pty->connectToServer(host, port.toInt());
}
