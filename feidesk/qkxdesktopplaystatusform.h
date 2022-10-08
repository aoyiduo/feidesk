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

#ifndef QKXDESKTOPPLAYSTATUSFORM_H
#define QKXDESKTOPPLAYSTATUSFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxDesktopPlayStatusForm;
}


class QKxLoadingBarWidget;
class QKxDesktopPlayStatusForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxDesktopPlayStatusForm(QWidget *parent = 0);
    ~QKxDesktopPlayStatusForm();
    void setSplashMessage(const QString &msg);
    void setMessage(const QString& msg);
    void setTryButtonVisible(bool on);
signals:
    void tryAgain();
    void showArrived();
private:
    void showEvent(QShowEvent *event);
private:
    Ui::QKxDesktopPlayStatusForm *ui;
    QPointer<QKxLoadingBarWidget> m_loading;
    qint64 m_lastSplash;
};

#endif // QKXDESKTOPPLAYSTATUSFORM_H
