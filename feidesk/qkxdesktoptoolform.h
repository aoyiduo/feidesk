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

#ifndef QKXDESKTOPTOOLFORM_H
#define QKXDESKTOPTOOLFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxDesktopToolForm;
}

class QMenu;
class QKxVncWidget;
class QKxDesktopToolForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxDesktopToolForm(QKxVncWidget *vnc, QWidget *parent = 0);
    ~QKxDesktopToolForm();

    QSize sizeHint() const;
signals:
    void enter();
    void leave();
    void nextScreenRatio();
    void nextScreenMode();
    void blackScreen();
    void lockScreen();
    void qualityLevel(int lv);
    void sizeChanged(const QSize& sz);
    void ftpClicked();
private slots:    
    void onQualityMenuAboutToShow();
    void onQualityLevelSelect(QAction *chk);
    void onQualityMenuActived();
    void onExitFullScreen();
    void onExitRemoteDesktop();
    void onFullScreen();
    void onMinimizeWindow();
    void onScreenCountChanged(int cnt);
    void onMessageSupport(int cnt);

    void onAudioClicked();
    void onAudioStateArrived(int state);
    void onPrivacyClicked();
    void onPrivacyStateArrived(int state);
private:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    bool eventFilter(QObject *w, QEvent *e);
    void resizeEvent(QResizeEvent *e);
private:
    Q_INVOKABLE void updateStatus();
    Q_INVOKABLE void resetLayout();
private:
    Ui::QKxDesktopToolForm *ui;
    QPointer<QKxVncWidget> m_vnc;
    QPointer<QMenu> m_qlvMenu;
    QPointer<QAction> m_qlv0;
    QPointer<QAction> m_qlv1;
    QPointer<QAction> m_qlv2;
    QPointer<QAction> m_qlv3;
    QPointer<QAction> m_qlv4;
    QPointer<QAction> m_qlv5;
    QPointer<QAction> m_qlv6;
    QPointer<QAction> m_qlv7;
    QPointer<QAction> m_qlv8;
    QPointer<QAction> m_qlv9;
    QPointer<QAction> m_qlv10;
    QPointer<QAction> m_qlv11;
};

#endif // QKXDESKTOPTOOLFORM_H
