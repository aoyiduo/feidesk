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

#include "qkxdevicetreemodel.h"
#include "qkxpowerapplication.h"
#include "qkxmodel_share.h"

#include <QDebug>
#include <QSysInfo>
#include <QDateTime>

#define DEVICE_ADD              (1)
#define DEVICE_ONLINE           (2)
#define DEVICE_OFFLINE          (4)
#define DEVICE_REMOVE           (8)


QKxDeviceTreeModel::QKxDeviceTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    QStandardItem *root = invisibleRootItem();
    m_recent = new QStandardItem(tr("Recent connection"));
    m_recent->setData(1, ROLE_CATEGORY);
    root->appendRow(m_recent);
    m_online = new QStandardItem(tr("Online devices"));
    m_online->setData(2, ROLE_CATEGORY);
    root->appendRow(m_online);
    m_offline = new QStandardItem(tr("Offline devices"));
    m_offline->setData(3, ROLE_CATEGORY);
    root->appendRow(m_offline);

    m_myIcon = QIcon(QPixmap(":/resource/skin/my.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
    m_onIcon = QIcon(QPixmap(":/resource/skin/online2.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
    m_offIcon = QIcon(QPixmap(":/resource/skin/offline2.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
    m_winx = QIcon(QPixmap(":/resource/skin/winx.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
    m_macos = QIcon(QPixmap(":/resource/skin/macos.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
    m_ubuntu = QIcon(QPixmap(":/resource/skin/ubuntu2.png").scaled(48, 48, Qt::KeepAspectRatio ,Qt::SmoothTransformation));
}

QKxDeviceTreeModel::~QKxDeviceTreeModel()
{

}

void QKxDeviceTreeModel::onDeviceListArrived(const QVariantList &lst)
{
    beginResetModel();
    m_online->removeRows(0, m_online->rowCount());
    m_offline->removeRows(0, m_offline->rowCount());
    m_recent->removeRows(0, m_recent->rowCount());
    QString deviceName = QSysInfo::machineHostName() + QString("[%1]").arg(tr("Local"));
    qint64 deviceId = QKxPowerApplication::instance()->deviceId();
    qint64 loginLast = QDateTime::currentSecsSinceEpoch();
    qint16 online = 99;

    QStandardItem *child = new QStandardItem(deviceName);
    child->setData(deviceId, ROLE_DEVICE_ID);
    child->setData(loginLast, ROLE_LOGIN_LAST);
    child->setData(online, ROLE_ONLINE);
    m_online->appendRow(child);
    for(int i = 0; i < lst.length(); i++) {
       QVariantMap dm = lst.at(i).toMap();
       qint64 deviceId = dm.value("deviceId").toLongLong();
       QString name = dm.value("deviceName").toString();
       quint32 loginLast = dm.value("loginLast").toUInt();
       qint8 online = dm.value("online").toInt();
       quint64 serviceFlag = dm.value("serviceFlag").toULongLong();
       qint8 platform = dm.value("platform").toInt();
       qint8 natType = dm.value("natType").toInt();

       QStandardItem *child = new QStandardItem(name);
       child->setData(deviceId, ROLE_DEVICE_ID);
       child->setData(loginLast, ROLE_LOGIN_LAST);
       child->setData(online, ROLE_ONLINE);
       child->setData(serviceFlag, ROLE_SERVICE_FLAG);
       child->setData(platform, ROLE_PLATFORM);
       child->setData(natType, ROLE_NAT_TYPE);
       if(online == 1) {
           m_online->appendRow(child);
       }else{
           m_offline->appendRow(child);
       }
    }
    QStandardItem *root = invisibleRootItem();
    endResetModel();
    hiddenArrived(0, !m_recent->hasChildren());
    hiddenArrived(2, !m_offline->hasChildren());
}

void QKxDeviceTreeModel::onDeviceChanged(int changed, const QVariant &m)
{
    qDebug() << "onDeviceChanged" << changed << m;
    _onDeviceChanged(changed, m);
    hiddenArrived(0, !m_recent->hasChildren());
    hiddenArrived(2, !m_offline->hasChildren());
}

void QKxDeviceTreeModel::onServiceFlagArrived(qint64 devid, quint64 flag)
{
    for(int r = 0; r < m_online->rowCount(); r++) {
        QStandardItem *child = m_online->child(r);
        qint64 id_hit = child->data(ROLE_DEVICE_ID).toLongLong();
        if(id_hit == devid) {
            child->setData(flag, ROLE_SERVICE_FLAG);
            break;
        }
    }
}

QVariant QKxDeviceTreeModel::data(const QModelIndex &index, int role) const
{
    QStandardItem *item = itemFromIndex(index);
    if(item == nullptr) {
        return 0;
    }
    if(role == Qt::DecorationRole) {
        if(item == m_recent || item == m_online || item == m_offline) {
            return QVariant();
        }
        int online = item->data(ROLE_ONLINE).toInt();
        if( online > 1) {
            return m_myIcon;
        }else if(online == 1) {
            int platform = item->data(ROLE_PLATFORM).toInt();
            if(platform == 1) {
                // window
                return m_winx;
            }else if(platform == 2) {
                // unix.
                return m_ubuntu;
            }else if(platform == 3) {
                // mac
                return m_macos;
            }else if(platform == 4) {
                // android
            }else {
                // other.
            }
            return m_onIcon;
        }
        return m_offIcon;
    }else if(role == Qt::FontRole) {
        if(item == m_recent || item == m_online || item == m_offline) {
            QFont ft = QGuiApplication::font();
            ft.setBold(true);
            ft.setPointSize(15);
            return ft;
        }
    }else if(role == Qt::TextColorRole) {
        if(item == m_recent || item == m_online) {
            return QColor(Qt::black);
        }else if(item == m_offline){
            return QColor(Qt::lightGray);
        }else {
            int online = item->data(ROLE_ONLINE).toInt();
            if(online > 0) {
                return QColor(Qt::black);
            }
            return QColor(Qt::lightGray);
        }
    }else if(role == Qt::SizeHintRole) {
        return QSize(-1, 30);
    }
    return QStandardItemModel::data(index, role);
}

bool QKxDeviceTreeModel::removeDevice(QStandardItem *node, qint64 did)
{
    for(int i = 0; i < node->rowCount(); i++) {
        QStandardItem *child = node->child(i);
        qint64 hid = child->data(ROLE_DEVICE_ID).toLongLong();
        if(hid == did) {
            node->removeRow(i);
            return true;
        }
    }
    return false;
}

void QKxDeviceTreeModel::_onDeviceChanged(int changed, const QVariant &m)
{
    QVariantMap dm = m.toMap();
    qint64 deviceId = dm.value("deviceId").toLongLong();

    removeDevice(m_recent, deviceId);
    removeDevice(m_online, deviceId);
    removeDevice(m_offline, deviceId);
    if(changed & DEVICE_REMOVE) {
        return;
    }
    QString name = dm.value("deviceName").toString();
    quint32 loginLast = dm.value("loginLast").toUInt();
    qint8 online = dm.value("online").toInt();
    quint32 serviceFlag = dm.value("serviceFlag").toUInt();
    qint8 platform = dm.value("platform").toInt();
    qint8 natType = dm.value("natType").toInt();

    QStandardItem *child = new QStandardItem(name);
    child->setData(deviceId, ROLE_DEVICE_ID);
    child->setData(loginLast, ROLE_LOGIN_LAST);
    child->setData(online, ROLE_ONLINE);
    child->setData(serviceFlag, ROLE_SERVICE_FLAG);
    child->setData(platform, ROLE_PLATFORM);
    child->setData(natType, ROLE_NAT_TYPE);
    if(online > 0) {
        int cnt = m_online->rowCount();
        m_online->insertRow(cnt > 0 ? 1 : 0, child);
    }else{
        m_offline->insertRow(0, child);
    }
}
