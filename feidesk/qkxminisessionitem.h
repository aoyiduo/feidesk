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

#ifndef QKXMINISESSIONITEM_H
#define QKXMINISESSIONITEM_H

#include <QWidget>
#include <QVariantMap>
#include <QList>

namespace Ui {
class QKxMiniSessionItem;
}

class QToolButton;
class QKxMiniSessionItem : public QWidget
{
    Q_OBJECT

public:
    explicit QKxMiniSessionItem(qint64 deviceId, const QVariantMap& data, QWidget *parent = 0);
    ~QKxMiniSessionItem();
    void addService(qint8 type);
    void removeService(qint8 type);
signals:
    void aboutToClose(qint64 devid);
private:
    void paintEvent(QPaintEvent *e);
private slots:
    void onCloseButtonClicked();
private:
    Ui::QKxMiniSessionItem *ui;
    QVariantMap m_data;
    qint64 m_deviceId;
    QList<QToolButton*> m_tools;
};

#endif // QKXMINISESSIONITEM_H
