//sgn

#include <iostream>
#include "jsonmodel.h"

JSONModel::JSONModel() : QAbstractTableModel() { }
JSONModel::~JSONModel() { }


QVariant JSONModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    QString strColumn = QString("Head_") + QString::number(section);
    return strColumn;
}

int JSONModel::rowCount(const QModelIndex &parent) const {
    return 50;
}

int JSONModel::columnCount(const QModelIndex &parent) const {
    return 15;
}

QVariant JSONModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole) return QVariant();
    QString strColumn = QString("Cell_") + QString::number(index.row()) + QString("_") + QString::number(index.column());
    return strColumn;
}
