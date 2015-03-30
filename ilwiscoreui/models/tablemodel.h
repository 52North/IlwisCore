#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
class Table;
typedef IlwisData<Table> ITable;
}
class TableCellModel;

class ILWISCOREUISHARED_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int columnCount READ getColumnCount NOTIFY columnCountChanged)
    Q_PROPERTY(int recordCount READ recordCount NOTIFY recordCountChanged)

public:
    TableModel();
    TableModel(const Ilwis::Resource& resource, QObject * parent);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int recordCount() const;
    int columnCount(const QModelIndex &/*parent*/) const;
    int getColumnCount() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Q_INVOKABLE QString roleName(int index) const;
    Q_INVOKABLE int defaultWidth(int index) const;
    Q_INVOKABLE bool isColumnSelected(quint32 index) const;
    Q_INVOKABLE void selectColumn(quint32 index);

    ~TableModel();
signals:
  void columnCountChanged();
  void recordCountChanged();
private:
    Ilwis::ITable _table;
    std::vector<quint32> _selectedColumns;
};

#endif // TABLEMODEL_H
