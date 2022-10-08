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

#ifndef QKXFILETRANSFERSTATUSFORM_H
#define QKXFILETRANSFERSTATUSFORM_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxFileTransferStatusForm;
}


class QKxLoadingBarWidget;
class QKxFileTransferStatusForm : public QWidget
{
    Q_OBJECT

public:
    explicit QKxFileTransferStatusForm(QWidget *parent = 0);
    ~QKxFileTransferStatusForm();
    void setSplashMessage(const QString& msg);
    void setMessage(const QString& msg);
    void setTryButtonVisible(bool on);
signals:
    void tryAgain();
private:
    Ui::QKxFileTransferStatusForm *ui;
    QPointer<QKxLoadingBarWidget> m_loading;
    qint64 m_lastSplash;
};

#endif // QKXFILETRANSFERSTATUSFORM_H
