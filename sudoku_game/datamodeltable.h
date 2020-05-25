#ifndef DATAMODELTABLE_H
#define DATAMODELTABLE_H

#include <QObject>
#include <QAbstractItemModel>
#include <QStringList>

class dataModelTable : public QAbstractItemModel
{
    QStringList** m_data;
    int sizeTable = 0;
public:
    dataModelTable(QObject *parent, QStringList List);

    // Model interface implementation

    virtual int columnCount(const QModelIndex &parent) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &index) const;

public slots:
    void addValue(const QString &value);

};

#endif // DATAMODELTABLE_H
