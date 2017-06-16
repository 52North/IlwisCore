#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
class Table;
typedef IlwisData<Table> ITable;
namespace Desktop{
    class TableOperation;
}
}
class ColumnModel;


class ILWISCOREUISHARED_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int columnCount READ getColumnCount NOTIFY columnCountChanged)
    Q_PROPERTY(int recordCount READ recordCount NOTIFY recordCountChanged)
    Q_PROPERTY(QQmlListProperty<ColumnModel> columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString  id READ id CONSTANT)
    Q_PROPERTY(QQmlListProperty<Ilwis::Desktop::TableOperation> operations READ operations NOTIFY operationsChanged)

public:
    TableModel();
    TableModel(const Ilwis::Resource& resource, QObject * parent);
    TableModel(const Ilwis::ITable &tbl, QObject *parent);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int recordCount() const;
    int columnCount(const QModelIndex &/*parent*/) const;
    int getColumnCount() const;
    void order(const std::vector<quint32>& neworder);
    const std::vector<quint32>& order() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    Ilwis::ITable table() const;
    QString url() const;
    QString id() const;

    Q_INVOKABLE QString roleName(int index) const;
    Q_INVOKABLE int defaultWidth(int index) const;
    Q_INVOKABLE bool isColumnSelected(quint32 index) const;
    Q_INVOKABLE void selectColumn(quint32 index, bool yesno);
    Q_INVOKABLE void update();
    Q_INVOKABLE bool isNumericalColumn(int index) const;
    void updateColumns();

    ~TableModel();

    QQmlListProperty<Ilwis::Desktop::TableOperation> operations();

signals:
  void columnCountChanged();
  void recordCountChanged();
  void columnsChanged();
  void operationsChanged();

private:
    QQmlListProperty<ColumnModel> columns();
    void setColumns();
    Ilwis::ITable _table;
    QList<ColumnModel *> _columns;
    std::vector<quint32> _order;
     QList<Ilwis::Desktop::TableOperation *> _operations;
};



#endif // TABLEMODEL_H
