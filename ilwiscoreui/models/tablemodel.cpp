#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "tablemodel.h"
#include "columnmodel.h"
#include "mastercatalog.h"

using namespace Ilwis;
TableModel::TableModel()
{

}

void TableModel::setColumns()
{
    _columns = QList<ColumnModel *>();
    _columns.push_back(new ColumnModel(this, TR("record"),"first"));
    _order.resize(_table->recordCount());
    for(int i =0; i < _order.size(); ++i)
        _order[i] = i;
    for(int i=0; i < _table->columnCount(); ++i){
        _columns.push_back(new ColumnModel(this, i));
    }
}

TableModel::TableModel(const Ilwis::Resource &resource, QObject *parent): QAbstractTableModel(parent)
{
    if ( resource.isValid()){
        _table = Ilwis::ITable(resource);
        setColumns();

    }
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if ( _table.isValid())    {
        return _table->recordCount();
    }
    return 0;
}

int TableModel::recordCount() const
{
    if ( _table.isValid())    {
        return _table->recordCount();
    }
    return 0;
}

int TableModel::columnCount(const QModelIndex &) const
{
    if ( _table.isValid())    {
        return _columns.size();
    }
    return 0;
}

int TableModel::getColumnCount() const
{
    return columnCount(QModelIndex()); //  the "real" column count
}

void TableModel::order(const std::vector<quint32> &neworder)
{
    if ( neworder.size() != _table->recordCount())
        return;
    for(quint32 index : neworder){
        if ( index >= _table->recordCount())
            return;
    }
    _order = neworder;
}

const std::vector<quint32> &TableModel::order() const
{
    return _order;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    QVariant v;
    if ( _table.isValid()){
        quint32 baseRole = Qt::UserRole + 1;
        if ( role - baseRole == 0){
            v = index.row();
        }
        else if ( index.row() < _table->recordCount()) {
            if ( _order.size() != _table->recordCount() ){
                 // not all tables might have set their record count correct at creation time. some
                // system don't have this metadata available. If not correct we must initialize the order now
                std::vector<quint32> order(_table->recordCount());
                for(int i =0; i < order.size(); ++i)
                    order[i] = i;
                const_cast<TableModel *>(this)->order(order);
            }
            quint32 ind = _order[ index.row()];
            v = _table->cell(role - baseRole - 1 ,ind, false);
            if ( v.toDouble() == rUNDEF || v.toDouble() == iUNDEF)
                v = sUNDEF;
        }
    }

    return v;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QVariant v;
    if ( _table.isValid()){
        quint32 baseRole = Qt::UserRole + 1;
        if ( role - baseRole == 0){
            return true;
        }
        else if ( index.row() < _table->recordCount()) {
            quint32 ind = _order[ index.row()];
            _table->setCell(role - baseRole - 1 ,ind, value);
        }
    }

    return true;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( _table.isValid() && section < _columns.size()){
        return  _columns[section]->attributename();
    }
    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    QHash<int, QByteArray> rolename;
    quint32 baseRole = Qt::UserRole + 1;
    for(int i = 0; i < _columns.size(); ++i)    {
        rolename[i + baseRole] = _columns[i]->role().toLocal8Bit();
    }
    return rolename;

}

ITable TableModel::table() const
{
    return _table;
}

QString TableModel::url() const
{
    if ( _table.isValid())
        return _table->source().url().toString();
    return "";
}

QString TableModel::roleName(int index) const
{
    if ( index >= 0 && index < _columns.size())
        return _columns[index]->role();
    return "";
}

int TableModel::defaultWidth(int index) const
{
    if ( index >= 0 && index < _columns.size())
        return _columns[index]->defaultWidth();
    return 0;
}

bool TableModel::isColumnSelected(quint32 index) const
{
    if ( index < _columns.size())    {
        return _columns[index]->isSelected();
    }
    return false;
}

void TableModel::selectColumn(quint32 index, bool yesno)
{
    if ( index < _columns.size())    {
        _columns[index]->selected(yesno);
    }
}

void TableModel::update()
{
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(600, 4);

    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();
}

void TableModel::updateColumns()
{
    setColumns();
    emit columnsChanged();
    update();
}

TableModel::~TableModel()
{

}

QQmlListProperty<ColumnModel> TableModel::columns()
{
    return QQmlListProperty<ColumnModel>(this, _columns) ;
}
