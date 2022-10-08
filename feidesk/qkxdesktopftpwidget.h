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

#ifndef QKXDESKTOPFTPWIDGET_H
#define QKXDESKTOPFTPWIDGET_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class QKxDesktopFtpWidget;
}

class QKxFtpTransferWidget;
class QKxVncWidget;
class QKxFtpRequest;

class QKxDesktopFtpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QKxDesktopFtpWidget(const QString& devid, QKxVncWidget *vnc, QWidget *parent = nullptr);
    ~QKxDesktopFtpWidget();

private slots:
    void onAboutToClose();
    void onFinished();
private:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *e);
private:
    Ui::QKxDesktopFtpWidget *ui;
    QPoint m_dragPosition;
    QPointer<QKxFtpTransferWidget> m_transfer;
    QPointer<QKxVncWidget> m_vnc;
    QPointer<QKxFtpRequest> m_ftpRequest;
};

#endif // QKXDESKTOPFTPWIDGET_H
