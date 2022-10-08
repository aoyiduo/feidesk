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

#ifndef QKXCHATSTATUSFORM_H
#define QKXCHATSTATUSFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxChatStatusForm;
}


class QKxLoadingBarWidget;
class QKxChatStatusForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxChatStatusForm(QWidget *parent = 0);
    ~QKxChatStatusForm();
    void setSplashMessage(const QString& msg);
    void setFixMessage(const QString& msg);
    void setMessage(const QString& msg);
    void setTryButtonVisible(bool on);
signals:
    void tryAgain();
private:
    Ui::QKxChatStatusForm *ui;
    QPointer<QKxLoadingBarWidget> m_loading;
    qint64 m_lastSplash;
    QString m_fixMessage;
};

#endif // QKXCHATSTATUSFORM_H
