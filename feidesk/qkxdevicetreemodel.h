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

#ifndef QKXDEVICETREEMODEL_H
#define QKXDEVICETREEMODEL_H

#include <QStandardItemModel>


class QKxDeviceTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit QKxDeviceTreeModel(QObject *parent = nullptr);
    ~QKxDeviceTreeModel();

signals:
    void hiddenArrived(int row, bool on);
private slots:
    void onDeviceListArrived(const QVariantList& lst);
    void onDeviceChanged(int changed, const QVariant& m);    
    void onServiceFlagArrived(qint64 devid, quint64 flag);
private:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool removeDevice(QStandardItem *parent, qint64 did);
    void _onDeviceChanged(int changed, const QVariant& m);
private:
    QStandardItem *m_recent;
    QStandardItem *m_online;
    QStandardItem *m_offline;
    QIcon m_myIcon, m_onIcon, m_offIcon;
    QIcon m_ubuntu, m_winx, m_macos;
};

#endif // QKXDEVICETREEMODEL_H
