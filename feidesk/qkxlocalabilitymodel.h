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

#ifndef QKXLOCALABILITYMODEL_H
#define QKXLOCALABILITYMODEL_H

#include <QAbstractListModel>
#include <QFont>
#include <QIcon>
#include <QDataStream>


class QKxLocalAbilityModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit QKxLocalAbilityModel(QObject *parent = nullptr);
    ~QKxLocalAbilityModel();
private slots:
    void onServiceFlagActive(quint64 flag);
private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::DropActions supportedDropActions() const override;
private:
    int totalCount() const;
private:
    quint32 m_serviceFlag;
    QIcon m_icoFtp, m_icoVnc, m_icoTermimal, m_icoPrinter, m_icoChat, m_icoCamera;
    QIcon m_icoFtp2, m_icoVnc2, m_icoTermimal2, m_icoPrinter2, m_icoChat2, m_icoCamera2;
};

#endif // QKXLOCALABILITYMODEL_H
