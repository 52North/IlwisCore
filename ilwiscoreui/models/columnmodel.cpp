#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "domain.h"
#include "factory.h"
#include "abstractfactory.h"
#include "../tableoperations/tableoperation.h"
#include "../tableoperations/tableoperationfactory.h"
#include "tablemodel.h"
#include "columnmodel.h"

ColumnModel::ColumnModel() :AttributeModel()
{

}

ColumnModel::ColumnModel(TableModel *tblModel, quint32 index) : AttributeModel(tblModel->table()->columndefinition(index),tblModel, tblModel->table())
{
    _name = tblModel->table()->columndefinition(index).name();
    _role = _name;

    IlwisTypes tp = tblModel->table()->columndefinition(index).datadef().domain()->valueType();
    if ( hasType(tp, itSTRING))
         _defaultWidth = 100;
    else if ( hasType(tp, itINTEGER))
         _defaultWidth = 30;
    else if ( hasType(tp, itFLOAT | itDOUBLE))
         _defaultWidth = 70;
     _defaultWidth = 80;

     auto *factory = Ilwis::kernel()->factory<Ilwis::Desktop::TableOperationFactory>("ilwis::tableoperationfactory");
     if ( factory){
         QVariantMap parameters = {{"columnindex",index}};
         _operations = factory->selectedOperations(tblModel, parameters);
         for(auto iter = _operations.begin(); iter != _operations.end(); ++iter)
             (*iter)->setParent(this);
     }
     _columnIndex = index;
}


ColumnModel::ColumnModel(TableModel *tblModel, const QString &name, const QString& rle) :
    AttributeModel(Ilwis::ColumnDefinition(),tblModel, tblModel->table()),
    _name(name), _role(rle)
{
    if ( rle == "first"){
        int rc = tblModel->table()->recordCount();
        if ( rc < 100)
            _defaultWidth = 30;
        else if ( rc < 1000)
            _defaultWidth = 45;
        else if ( rc < 10000)
            _defaultWidth = 55;
        else
            _defaultWidth  = 65;
    }
}

QString ColumnModel::attributename() const
{
    return _name;
}

QString ColumnModel::role() const
{
    return _role;
}

bool ColumnModel::isSelected() const
{
    return _selected;
}

void ColumnModel::selected(bool yesno)
{
    _selected = yesno;
}

bool ColumnModel::isSortColumn() const
{
    return _sortColumn;
}

void ColumnModel::sortColumn(bool yesno)
{
    _sortColumn = yesno;
}

quint32 ColumnModel::defaultWidth() const
{
    return _defaultWidth;
}

quint32 ColumnModel::columnIndex() const
{
    return _columnIndex;
}

QQmlListProperty<Ilwis::Desktop::TableOperation> ColumnModel::operations()
{
    return QQmlListProperty<Ilwis::Desktop::TableOperation>(this, _operations);
}

Ilwis::Desktop::TableOperation *ColumnModel::operation(quint32 index)
{
    if ( index < _operations.size()){
        return _operations[index];
    }
    return 0;
}

