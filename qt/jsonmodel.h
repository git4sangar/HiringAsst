#ifndef JSONMODEL_H
#define JSONMODEL_H

//sgn
#include <QAbstractTableModel>

class JSONModel : public QAbstractTableModel
{
public:
    JSONModel();
    virtual ~JSONModel();

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // JSONMODEL_H
