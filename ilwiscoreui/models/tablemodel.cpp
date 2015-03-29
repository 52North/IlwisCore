#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "tablemodel.h"
#include "mastercatalog.h"

TableModel::TableModel()
{

}

TableModel::TableModel(const Ilwis::Resource &resource, QObject *parent): QAbstractTableModel(parent)
{
    if ( resource.isValid()){
        _table = Ilwis::ITable(resource);
    }
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if ( _table.isValid())    {
        return _table->recordCount() + 5;
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
        return _table->columnCount() + 1; // add one for the column with the record nr
    }
    return 0;
}

int TableModel::getColumnCount() const
{
    return columnCount(QModelIndex()) - 1; //  the "real" column count
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if ( _table.isValid()){
        quint32 baseRole = Qt::UserRole + 1;
        if ( role - baseRole == 0){
            return QVariant(index.row());
        }
        if ( index.row() < _table->recordCount()) {

            return _table->cell(role - baseRole - 1, index.row(), false);
        }else{
            return "test";
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( _table.isValid()){
        return  _table->columndefinition(section).name();
    }
    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    QHash<int, QByteArray> rolename;
    quint32 baseRole = Qt::UserRole + 1;
    rolename[baseRole] = "first";
    for(int i = 0; i < _table->columnCount(); ++i)    {
        rolename[i + baseRole + 1] = _table->columndefinition(i).name().toLocal8Bit();
    }
    return rolename;

}

QString TableModel::roleName(int index) const
{
    if ( _table.isValid())    {
        return _table->columndefinition(index).name();
    }
    return "";
}

int TableModel::defaultWidth(int index) const
{
    if ( _table.isValid())    {
        IlwisTypes tp = _table->columndefinition(index).datadef().domain()->valueType();
        if ( hasType(tp, itSTRING))
            return 100;
        if ( hasType(tp, itINTEGER))
            return 30;
        if ( hasType(tp, itFLOAT | itDOUBLE))
            return 50;
        return 70;
    }
    return 0;
}

bool TableModel::isColumnSelected(quint32 index) const
{
    return std::find(_selectedColumns.begin(), _selectedColumns.end(),index) != _selectedColumns.end();
}

void TableModel::selectColumn(quint32 index)
{
    if ( index < _table->columnCount())    {
        auto iter = std::find(_selectedColumns.begin(), _selectedColumns.end(),index);
        if ( iter != _selectedColumns.end())
            _selectedColumns.erase(iter);
        else
            _selectedColumns.push_back(index);

    }
}

TableModel::~TableModel()
{

}

