#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "basetable.h"

using namespace Ilwis;

BaseTable::BaseTable() : Table(), _rows(0), _columns(0), _dataloaded(false)
{
}

BaseTable::BaseTable(const Resource& res) : Table(res), _rows(0), _columns(0),_dataloaded(false) {

}

BaseTable::~BaseTable()
{
}

quint32 BaseTable::rows() const
{
    return _rows;
}

quint32 BaseTable::columns() const
{
    return _columns;
}

void BaseTable::setRows(quint32 r)
{
    _rows = r;
}

bool BaseTable::createTable()
{
    if (!isValid()) {
        kernel()->issues()->log(TR("Not created, Table %1 already exists").arg(name()), IssueObject::itWarning);
        return false;
    }
    if ( _columnDefinitionsByName.size() ==  0) {
        kernel()->issues()->log(TR("Trying to create table %1 with zero columns").arg(name()));
        return false;
    }
    return true;
}

bool BaseTable::addColumn(const ColumnDefinition& def){
    if ( _columnDefinitionsByName.contains(def.name())) {
        kernel()->issues()->log(TR("Adding duplicate column %1").arg(name()),IssueObject::itWarning);
        return false;
    }
    _columnDefinitionsByName[def.name()] = def;
    _columnDefinitionsByIndex[def.id()] = _columnDefinitionsByName[def.name()];
    _columns = _columnDefinitionsByName.size();

    return true;
}

bool BaseTable::addColumn(const QString &name, const IDomain& domain)
{
    if ( _columnDefinitionsByName.contains(name)) {
        kernel()->issues()->log(TR("Adding duplicate column %1").arg(name),IssueObject::itWarning);
        return false;
    }
    _columnDefinitionsByName[name] = ColumnDefinition(name, domain,_columnDefinitionsByName.size());
    _columnDefinitionsByIndex[_columnDefinitionsByName[name].id()] = _columnDefinitionsByName[name];
    _columns = _columnDefinitionsByName.size();

    return true;
}

bool BaseTable::addColumn(const QString &name, const QString &domainname)
{
    IDomain dom;
    if(!dom.prepare(domainname))
        return false;
    return addColumn(name, dom);
}

IlwisTypes BaseTable::ilwisType() const
{
    return itTABLE;
}

ColumnDefinition BaseTable::columndefinition(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter != _columnDefinitionsByName.end())
        return iter.value();
    return ColumnDefinition();
}

ColumnDefinition BaseTable::columndefinition(quint32 index) const
{
    auto iter = _columnDefinitionsByIndex.find(index);
    if ( iter != _columnDefinitionsByIndex.end())
        return iter.value();
    return ColumnDefinition();
}


bool BaseTable::prepare()
{
    if (!IlwisObject::prepare())
        return false;

    return true;
}

bool BaseTable::isValid() const
{
    return _rows != 0 && _columns != 0;
}

ColumnDefinition &BaseTable::columndefinition(quint32 index)
{
    return _columnDefinitionsByIndex[index]    ;
}

bool  BaseTable::initLoad() {
    if ( !this->isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg("table"));
        return false;
    }
    if (!_dataloaded) {
        _dataloaded = true; // to prevent other inits to pass through here
        if (! connector()->loadBinaryData(this)) {
            kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg("table", name()));
             _dataloaded = false;
            return false;
        }
    }
    return true;
}

quint32 BaseTable::columnIndex(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter == _columnDefinitionsByName.end()) {
        kernel()->issues()->log(TR(ERR_COLUMN_MISSING_2).arg(nme).arg(name()),IssueObject::itWarning);
        return iUNDEF;
    }
    return iter.value().id();
}





