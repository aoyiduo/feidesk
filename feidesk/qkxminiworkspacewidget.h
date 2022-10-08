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

#ifndef QKXMINIWORKSPACEWIDGET_H
#define QKXMINIWORKSPACEWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QMap>

namespace Ui {
class QKxMiniWorkSpaceWidget;
}

class QKxMiniSessionItem;
class QVBoxLayout;
class QKxMiniWorkspaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QKxMiniWorkspaceWidget(QWidget *parent = 0);
    ~QKxMiniWorkspaceWidget();
    void addSession(qint64 did, const QVariantMap& dm);
    void removeSession(qint64 did);

    void addService(qint64 did, qint8 type);
    void removeService(qint64 did, qint8 type);

    bool sessionEmpty();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *e);

private slots:
    void onAboutToClose();
    void onItemAboutToClose(qint64 devid);
    void onResetLayout();
private:
    void resetLayout();
private:
    Ui::QKxMiniWorkSpaceWidget *ui;
    QPoint m_dragPosition;

    QPointer<QVBoxLayout> m_sessionLayout;
    QMap<qint64, QPointer<QKxMiniSessionItem> > m_sessions;
};

#endif // QKXMINIWORKSPACEWIDGET_H
