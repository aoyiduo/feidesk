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

#include "qkxlocalabilitymodel.h"
#include "qkxutils.h"
#include "qkxsettingex.h"
#include "qkxnet_share.h"
#include "qkxmodel_share.h"

#include <QDebug>
#include <QFile>
#include <QDateTime>

#define ICON_SIZE      (64)

QKxLocalAbilityModel::QKxLocalAbilityModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_serviceFlag(0)
{
    m_icoCamera = QIcon(QPixmap(":/resource/skin/camera.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoFtp = QIcon(QPixmap(":/resource/skin/ftp3.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoVnc = QIcon(QPixmap(":/resource/skin/mstsc2.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoChat = QIcon(QPixmap(":/resource/skin/chat.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoTermimal = QIcon(QPixmap(":/resource/skin/terminal.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoPrinter = QIcon(QPixmap(":/resource/skin/printer.png").scaled(ICON_SIZE,ICON_SIZE));
    m_icoCamera2 = m_icoCamera.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
    m_icoFtp2 = m_icoFtp.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
    m_icoVnc2 = m_icoVnc.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
    m_icoChat2 = m_icoChat.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
    m_icoTermimal2 = m_icoTermimal.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
    m_icoPrinter2 = m_icoPrinter.pixmap(QSize(ICON_SIZE, ICON_SIZE), QIcon::Disabled);
}

QKxLocalAbilityModel::~QKxLocalAbilityModel()
{

}

void QKxLocalAbilityModel::onServiceFlagActive(quint64 flag)
{
    if(m_serviceFlag == flag) {
        return;
    }
    beginResetModel();
    m_serviceFlag = flag;
    endResetModel();
    qDebug() << "onServiceFlagActive:" << flag;
}

int QKxLocalAbilityModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return 0;
    }
    return 6;
}

QModelIndex QKxLocalAbilityModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (!idx.isValid() || column > 1 || row < 0)
        return QModelIndex();

    return createIndex(row, column);
}

QVariant QKxLocalAbilityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractListModel::headerData(section, orientation, role);
}

QVariant QKxLocalAbilityModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0) {
        return QVariant();
    }
    if (!index.isValid()){
        return QVariant();
    }
    if(role == Qt::SizeHintRole) {
        return QSize(ICON_SIZE, ICON_SIZE);
    }    
    if(role == Qt::DecorationRole) {
        if(index.column() != 0) {
            return QVariant();
        }
        int row = index.row();
        int flag = 1 << row;
        if((flag & m_serviceFlag) == SERVICE_FLAG_VNC) {
            return m_icoVnc;
        }else if((flag & m_serviceFlag) == SERVICE_FLAG_FTP) {
            return m_icoFtp;
        }else if((flag & m_serviceFlag) == SERVICE_FLAG_CHAT) {
            return m_icoChat;
        }else if((flag & m_serviceFlag) == SERVICE_FLAG_TERMINAL) {
            return m_icoTermimal;
        }else if((flag & m_serviceFlag) == SERVICE_FLAG_PRINTER) {
            return m_icoPrinter;
        }else if((flag & m_serviceFlag) == SERVICE_FLAG_CAMERA) {
            return m_icoCamera;
        }else if(flag == SERVICE_FLAG_VNC) {
            return m_icoVnc2;
        }else if(flag == SERVICE_FLAG_FTP) {
            return m_icoFtp2;
        }else if(flag == SERVICE_FLAG_CHAT) {
            return m_icoChat2;
        }else if(flag == SERVICE_FLAG_TERMINAL) {
            return m_icoTermimal2;
        }else if(flag == SERVICE_FLAG_PRINTER) {
            return m_icoPrinter2;
        }else if(flag == SERVICE_FLAG_CAMERA) {
            return m_icoCamera2;
        }
        return QVariant();
    }else if(role == Qt::TextColorRole) {
        if(index.column() != 0) {
            return QVariant();
        }
        int row = index.row();
        int flag = 1 << row;
        if(flag & m_serviceFlag) {
            return QColor(Qt::black);
        }
        return QColor(Qt::gray);
    }else if(role == Qt::DisplayRole) {
        int row = index.row();
        int flag = 1 << row;
        if(flag == SERVICE_FLAG_VNC) {
            return tr("Desktop");
        }else if(flag == SERVICE_FLAG_FTP) {
            return tr("Ftp");
        }else if(flag == SERVICE_FLAG_CHAT) {
            return tr("Chat");
        }else if(flag == SERVICE_FLAG_TERMINAL) {
            return tr("Terminal");
        }else if(flag == SERVICE_FLAG_PRINTER) {
            return tr("Printer");
        }else if(flag == SERVICE_FLAG_CAMERA) {
            return tr("Camera");
        }
        return QVariant();
    }else if(role == ROLE_SERVICE_TYPE) {
        int row = index.row();
        int flag = 1 << row;
        return flag;
    }else if(role == ROLE_SERVICE_RUNNING) {
        int row = index.row();
        int flag = 1 << row;
        return (flag & m_serviceFlag) > 0;
    }
    return QVariant();
}

bool QKxLocalAbilityModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags QKxLocalAbilityModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;;
}

bool QKxLocalAbilityModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "insertRow";
    return false;
}

bool QKxLocalAbilityModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent)){
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);


    endRemoveRows();

    return true;
}

void QKxLocalAbilityModel::sort(int column, Qt::SortOrder order)
{
    return QAbstractListModel::sort(column, order);
}

int QKxLocalAbilityModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

Qt::DropActions QKxLocalAbilityModel::supportedDropActions() const
{
    return QAbstractListModel::supportedDropActions();
}
