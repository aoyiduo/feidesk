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

#include "qkxdesktoptoolform.h"
#include "ui_qkxdesktoptoolform.h"
#include "qkxvncwidget.h"
#include "qkxmenu.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QMenu>
#include <QToolButton>
#include <QStyleOption>

QKxDesktopToolForm::QKxDesktopToolForm(QKxVncWidget *vnc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QKxDesktopToolForm)
  , m_vnc(vnc)
{
    ui->setupUi(this);
    setObjectName("deskopTool");
    setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(6);
    ui->btnNextScreen->setVisible(false);
    ui->btnNextScreen->setToolTip(tr("Switch to the next display"));
    ui->btnBlack->setVisible(false);
    ui->btnBlack->setToolTip(tr("Make the wallpaper black"));
    ui->btnLock->setVisible(false);
    ui->btnLock->setToolTip(tr("Trigger screen lock behavior"));
    ui->btnNextRatio->setVisible(false);
    ui->btnNextRatio->setToolTip(tr("Whether the image is stretched"));
    ui->btnQuality->setVisible(false);
    ui->btnQuality->setToolTip(tr("Encoding mode"));
    ui->btnAudioOff->setVisible(true);
    ui->btnAudioOff->setToolTip(tr("Remote audio is off"));
    ui->btnAudioOn->setVisible(false);
    ui->btnAudioOn->setToolTip(tr("Remote audio is on"));
    ui->btnAudioOff->setEnabled(true);
    ui->btnAudioOn->setEnabled(false);
    ui->btnPrivacyOff->setVisible(true);
    ui->btnPrivacyOff->setToolTip(tr(""));
    ui->btnPrivacyOn->setVisible(false);
    ui->btnPrivacyOn->setToolTip(tr("Remote desktop protection has been turned on"));
    ui->btnPrivacyOff->setEnabled(true);
    ui->btnPrivacyOff->setToolTip(tr("Remote desktop protection has been turned off"));
    ui->btnPrivacyOn->setEnabled(false);

    QObject::connect(m_vnc, SIGNAL(audioStateArrived(int)), this, SLOT(onAudioStateArrived(int)));
    QObject::connect(m_vnc, SIGNAL(privacyStateArrived(int)), this, SLOT(onPrivacyStateArrived(int)));

    QMenu *menu = new QKxMenu(this);
    m_qlvMenu = menu;
    QActionGroup *group = new QActionGroup(menu);
    QObject::connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onQualityLevelSelect(QAction*)));
    m_qlv0 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Auto mode"));
    m_qlv0->setVisible(false);
    m_qlv1 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Lossless mode"));
    m_qlv1->setToolTip(tr("The image quality is lossless, and the occupied bandwidth is very high, and the bandwidth is about 16Mbps"));
    m_qlv11 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("TureColor mode"));
    m_qlv11->setVisible(false);
    m_qlv7 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("HDTV mode"));
    m_qlv7->setToolTip(tr("The image quality is high and clear, the occupied bandwidth is slightly high, and the bandwidth is about 8Mbps"));
    m_qlv8 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("General clear mode"));
    m_qlv8->setToolTip(tr("The image quality is ordinary, and the occupied bandwidth is general, and the bandwidth is about 4Mbps"));
    m_qlv9 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Fast mode"));
    m_qlv9->setToolTip(tr("The image quality is slightly low, the occupied bandwidth is low, and the bandwidth is about 2Mbps"));
    m_qlv10 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Super fast mode"));
    m_qlv10->setToolTip(tr("The image quality is lowest, the occupied bandwidth is lowest, and the bandwidth is about 1Mbps"));
    m_qlv2 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Classics 16bit mode"));
    m_qlv3 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Classics 15bit mode"));
    m_qlv4 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Classics 8bit mode"));
    m_qlv5 = menu->addAction(QIcon(":/resource/skin/empty.png"), QObject::tr("Classics pallete mode"));
    group->addAction(m_qlv0);
    group->addAction(m_qlv1);
    group->addAction(m_qlv2);
    group->addAction(m_qlv3);
    group->addAction(m_qlv4);
    group->addAction(m_qlv5);
    group->addAction(m_qlv7);
    group->addAction(m_qlv8);
    group->addAction(m_qlv9);
    group->addAction(m_qlv10);
    group->addAction(m_qlv11);
    m_qlv0->setCheckable(true);
    m_qlv1->setCheckable(true);
    m_qlv2->setCheckable(true);
    m_qlv3->setCheckable(true);
    m_qlv4->setCheckable(true);
    m_qlv5->setCheckable(true);
    m_qlv7->setCheckable(true);
    m_qlv8->setCheckable(true);
    m_qlv9->setCheckable(true);
    m_qlv10->setCheckable(true);
    m_qlv11->setCheckable(true);
    m_qlv11->setChecked(true);
    ui->btnQuality->setMenu(menu);
    QObject::connect(menu, SIGNAL(aboutToShow()), this, SLOT(onQualityMenuAboutToShow()));
    QObject::connect(menu, SIGNAL(aboutToHide()), this, SIGNAL(leave()));
    QObject::connect(ui->btnQuality, SIGNAL(clicked(bool)), this, SLOT(onQualityMenuActived()));

    QObject::connect(ui->btnExitFull, SIGNAL(clicked(bool)), this, SLOT(onExitFullScreen()));
    QObject::connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(onExitRemoteDesktop()));
    QObject::connect(ui->btnFullScreen, SIGNAL(clicked(bool)), this, SLOT(onFullScreen()));
    QObject::connect(ui->btnMinimize, SIGNAL(clicked(bool)), this, SLOT(onMinimizeWindow()));
    QObject::connect(ui->btnNextRatio, SIGNAL(clicked(bool)), this, SIGNAL(nextScreenRatio()));
    QObject::connect(ui->btnNextScreen, SIGNAL(clicked(bool)), this, SIGNAL(nextScreenMode()));
    QObject::connect(ui->btnBlack, SIGNAL(clicked(bool)), this, SIGNAL(blackScreen()));
    QObject::connect(ui->btnLock, SIGNAL(clicked(bool)), this, SIGNAL(lockScreen()));
    QObject::connect(ui->btnAudioOff, SIGNAL(clicked(bool)), this, SLOT(onAudioClicked()));
    QObject::connect(ui->btnAudioOn, SIGNAL(clicked(bool)), this, SLOT(onAudioClicked()));
    QObject::connect(ui->btnPrivacyOff, SIGNAL(clicked(bool)), this, SLOT(onPrivacyClicked()));
    QObject::connect(ui->btnPrivacyOn, SIGNAL(clicked(bool)), this, SLOT(onPrivacyClicked()));
    QObject::connect(ui->btnFtp, SIGNAL(clicked(bool)), this, SIGNAL(ftpClicked()));

    parent->installEventFilter(this);
}

QKxDesktopToolForm::~QKxDesktopToolForm()
{
    delete ui;
}

QSize QKxDesktopToolForm::sizeHint() const
{
    QSize sz = QWidget::sizeHint();
    return sz;
}

void QKxDesktopToolForm::onQualityMenuAboutToShow()
{

}

void QKxDesktopToolForm::onQualityLevelSelect(QAction *pa)
{
    if(pa == m_qlv0) {
        emit qualityLevel(0);
    }else if(pa == m_qlv1) {
        emit qualityLevel(1);
    }else if(pa == m_qlv2) {
        emit qualityLevel(2);
    }else if(pa == m_qlv3) {
        emit qualityLevel(3);
    }else if(pa == m_qlv4) {
        emit qualityLevel(4);
    }else if(pa == m_qlv5) {
        emit qualityLevel(5);
    }else if(pa == m_qlv7) {
        emit qualityLevel(7);
    }else if(pa == m_qlv8) {
        emit qualityLevel(8);
    }else if(pa == m_qlv9) {
        emit qualityLevel(9);
    }else if(pa == m_qlv10) {
        emit qualityLevel(10);
    }else if(pa == m_qlv11) {
        emit qualityLevel(11);
    }
}

void QKxDesktopToolForm::onQualityMenuActived()
{
    QToolButton *btn = qobject_cast<QToolButton*>(sender());
    if(btn != nullptr) {
        QMenu *menu = btn->menu();
        QPoint pt = QCursor::pos();
        QAction* qlvs[] = {m_qlv0,m_qlv1,m_qlv2,m_qlv3,m_qlv4,m_qlv5, nullptr, m_qlv7,m_qlv8,m_qlv9,m_qlv10,m_qlv11};
        QKxVNC::EPixelFormat fmt = m_vnc->getPixelFormat();
        qDebug() << "onQualityMenuActived fmt:" << fmt;
        QAction *lv = qlvs[fmt];
        if(lv) {
            lv->setChecked(true);
        }else{
            qDebug() << "onQualityMenuActived: not hit lv";
        }
        menu->exec(pt);
    }
}

void QKxDesktopToolForm::onExitFullScreen()
{
    QWidget *w = topLevelWidget();
    w->showNormal();
}

void QKxDesktopToolForm::onExitRemoteDesktop()
{
    qint64 pid = QCoreApplication::applicationPid();
    QByteArray _ppid = qgetenv("NETHOST_PROCESS_PID");
    if(pid == _ppid.toLongLong()) {
        QWidget *w = topLevelWidget();
        w->close();
    }else{
        QCoreApplication::quit();
    }
}

void QKxDesktopToolForm::onFullScreen()
{
    QWidget *w = topLevelWidget();
    w->showFullScreen();
}

void QKxDesktopToolForm::onMinimizeWindow()
{
    QWidget *w = topLevelWidget();
    w->showMinimized();
}

void QKxDesktopToolForm::onScreenCountChanged(int cnt)
{
    ui->btnNextScreen->setVisible(cnt > 1);
    ui->btnBlack->setVisible(true);
    ui->btnLock->setVisible(true);
    ui->btnNextRatio->setVisible(true);
    ui->btnQuality->setVisible(true);
    resetLayout();
}

void QKxDesktopToolForm::onMessageSupport(int cnt)
{
    ui->btnFtp->setEnabled(m_vnc->supportMessage(QKxVNC::EMT_Ftp));
    ui->btnAudioOn->setEnabled(m_vnc->supportMessage(QKxVNC::EMT_PlayAudio));
    ui->btnAudioOff->setEnabled(m_vnc->supportMessage(QKxVNC::EMT_PlayAudio));
    ui->btnPrivacyOn->setEnabled(m_vnc->supportMessage(QKxVNC::EMT_PrivacyScreen));
    ui->btnPrivacyOff->setEnabled(m_vnc->supportMessage(QKxVNC::EMT_PrivacyScreen));
}

void QKxDesktopToolForm::onAudioClicked()
{
    if(ui->btnAudioOff == sender()) {
        m_vnc->setAudioEnabled(true);
    }else{
        m_vnc->setAudioEnabled(false);
    }
}

void QKxDesktopToolForm::onAudioStateArrived(int state)
{
    ui->btnAudioOff->setVisible(!state);
    ui->btnAudioOn->setVisible(state);
}

void QKxDesktopToolForm::onPrivacyClicked()
{
    if(ui->btnPrivacyOff == sender()) {
        m_vnc->setPrivacyScreenEnabled(true);
    }else{
        m_vnc->setPrivacyScreenEnabled(false);
    }
}

void QKxDesktopToolForm::onPrivacyStateArrived(int state)
{
    ui->btnPrivacyOff->setVisible(!state);
    ui->btnPrivacyOn->setVisible(state);
}

void QKxDesktopToolForm::paintEvent(QPaintEvent *e)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void QKxDesktopToolForm::enterEvent(QEvent *e)
{
    qDebug() << "enter";
    emit enter();
}

void QKxDesktopToolForm::leaveEvent(QEvent *e)
{
    qDebug() << "leave";
    if(m_qlvMenu->isVisible()) {
        return;
    }
    emit leave();
}

bool QKxDesktopToolForm::eventFilter(QObject *w, QEvent *e)
{
    QEvent::Type t = e->type();
    if(t == QEvent::Resize) {
        QMetaObject::invokeMethod(this, "updateStatus", Qt::QueuedConnection);
    }
    return QWidget::eventFilter(w, e);
}

void QKxDesktopToolForm::resizeEvent(QResizeEvent *e)
{
    emit sizeChanged(e->size());
    QWidget::resizeEvent(e);
}

void QKxDesktopToolForm::updateStatus()
{
    QWidget *w = topLevelWidget();
    ui->btnExitFull->setVisible(w->isFullScreen());
    ui->btnFullScreen->setVisible(!w->isFullScreen());
    resetLayout();
}

void QKxDesktopToolForm::resetLayout()
{
    QSize sz = sizeHint();
    resize(sz);
}
