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
        _columns.resize(_table->columnCount() + 1, false);
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
    return columnCount(QModelIndex()); //  the "real" column count
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

            v = _table->cell(role - baseRole - 1 , index.row(), false);
        }else{
            return "test";
        }
        qDebug() << index.row() << role - baseRole << role << v.toString();
    }

    return v;
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
    if ( index == 0)
        return "first";
    if ( _table.isValid())    {
        return _table->columndefinition(index - 1).name();
    }
    return "";
}

int TableModel::defaultWidth(int index) const
{
    try {
        if ( _table.isValid() && index >= 0 && index < _table->columnCount()){

            if ( index == 0){
                int rc = _table->recordCount();
                if ( rc < 100)
                    return 30;
                if ( rc < 1000)
                    return 45;
                if ( rc < 10000)
                    return 55;
                return 65;
            }
            IlwisTypes tp = _table->columndefinition(index).datadef().domain()->valueType();
            if ( hasType(tp, itSTRING))
                return 100;
            if ( hasType(tp, itINTEGER))
                return 30;
            if ( hasType(tp, itFLOAT | itDOUBLE))
                return 70;
            return 80;
        }
        return 40;
    }
    catch ( Ilwis::ErrorObject& err){

    }
    return 0;
}

bool TableModel::isColumnSelected(quint32 index) const
{
     if ( index < _table->columnCount())    {
         return _columns[index];
     }
     return false;
}

void TableModel::selectColumn(quint32 index, bool yesno)
{
    if ( index < _table->columnCount())    {
        _columns[index] = yesno;
    }
}

TableModel::~TableModel()
{

}

