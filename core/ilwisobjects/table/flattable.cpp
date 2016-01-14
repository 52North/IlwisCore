#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "domainitem.h"
#include "selectabletable.h"
#include "basetable.h"
#include "flattable.h"
#include "tablemerger.h"
#include "itemrange.h"

using namespace Ilwis;

FlatTable::FlatTable()
{
}

FlatTable::FlatTable(const Resource& resource) : BaseTable(resource){

}

FlatTable::~FlatTable()
{
    _datagrid.clear();
}

bool FlatTable::createTable()
{
    if(!BaseTable::createTable()) {
        return false;
    }
    for(unsigned int i=0; i < recordCount(); ++i) {
        _datagrid.push_back(std::vector<QVariant>(_attributeDefinition.definitionCount()));
    }
    return true;
}

Record& FlatTable::newRecord()
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("could not load %1")).arg(name()));
    }

    std::vector<QVariant> values;
    initRecord(values);
    record(NEW_RECORD, values);

    return _datagrid.back();
}

void FlatTable::removeRecord(quint32 rec)
{
    if ( rec < recordCount()){
        _datagrid.erase(_datagrid.begin() + rec);
    }
}

bool FlatTable::prepare(const IOOptions& options)
{
    return Table::prepare(options);
}

bool FlatTable::isValid() const
{
    return BaseTable::isValid();
}

bool FlatTable::addColumn(const QString &name, const IDomain &domain,const bool readonly)
{
    bool ok = BaseTable::addColumn(name, domain, readonly);
    if(!ok) {
        return false;
    }
    if ( isDataLoaded()){
        for(Record& row : _datagrid) {
            row.addColumn();
            row.changed(true);
        }
        initValuesColumn(name);
    }
    return true;

}

bool FlatTable::addColumn(const ColumnDefinition &def)
{
    bool ok = BaseTable::addColumn(def);
    if(!ok) {
        return false;
    }
    if (  isDataLoaded()) {
        for(Record& row : _datagrid) {
            row.addColumn();
            row.changed(true);
        }
        initValuesColumn(def.name());
    }
    return true;
}

std::vector<QVariant> FlatTable::column(quint32 index, quint32 start, quint32 stop) const {
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return std::vector<QVariant>();
    }

    if ( !isColumnIndexValid(index)) {
        return std::vector<QVariant>();
    }

    stop = std::min(stop, recordCount());
    start = std::max((quint32)0, start);
    std::vector<QVariant> data(stop - start);
    for(quint32 i=start; i < stop; ++i) {
        data[i - start] = _datagrid[i].cell(index);
    }
    return data;
}

std::vector<QVariant> FlatTable::column(const QString &nme, quint32 start, quint32 stop) const
{
    quint32 index = columnIndex(nme);
    return column(index, start, stop);

}

void FlatTable::column(quint32 index, const std::vector<QVariant> &vars, quint32 offset)
{
    if (index >= columnCount()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return ;
    }
    column( _attributeDefinition[index].name(), vars, offset);
}

void FlatTable::column(const QString &nme, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    quint32 index = columnIndex(nme);
    if ( !isColumnIndexValid(index)) {
        return ;
    }

    quint32 rec = offset;
    _attributeDefinition[index].changed(true);
    for(const QVariant& var : vars) {
        if ( rec < _datagrid.size()){
            _datagrid[rec].changed(true);
            _datagrid[rec++].cell(index, var);
        }
        else {
            _datagrid.push_back(std::vector<QVariant>(_attributeDefinition.definitionCount()));
            _datagrid[rec].changed(true);
            _datagrid[rec++].cell(index,checkInput(var,index));
            recordCount(_datagrid.size());
        }
    }

}

const Record& FlatTable::record(quint32 rec) const{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("failed load of table %1")).arg(name()));
    }

    if ( rec < recordCount() && _datagrid.size() != 0) {
         return _datagrid[rec];
    }
    throw ErrorObject(QString("Requested record number is not in the table").arg(rec));
}

Record& FlatTable::recordRef(quint32 rec)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("failed load of table %1")).arg(name()));
    }

    if ( rec < recordCount() && _datagrid.size() != 0) {
         return _datagrid[rec];
    }
    throw ErrorObject(QString("Requested record number is not in the table").arg(rec));
}

void FlatTable::record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset)
{

    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);
    if ( rec >=_datagrid.size() ) {
        _datagrid.push_back(std::vector<QVariant>(_attributeDefinition.definitionCount()));
        _datagrid.back().changed(true);
        // datagrid and recordcount might not have the same size as recordcount is the
        // number of records the metadata says that should be there and datagrid contains the actual
        // data that is loaded
        recordCount(std::max(recordCount(),_datagrid.size()));
        rec = _datagrid.size() - 1;
    }
    quint32 col = offset;
    int cols = std::min((quint32)vars.size() - offset, columnCount());
    for(const QVariant& var : vars) {
        if ( col < cols){
            _datagrid[rec].changed(true);
            _datagrid[rec].cell(col, checkInput(var, col));
            ++col;
        }
    }

}

QVariant FlatTable::cell(const QString& col, quint32 rec, bool asRaw) const {
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return QVariant();
    }
    quint32 index = columnIndex(col);
    return cell(index , rec, asRaw);
}

QVariant FlatTable::cell(const quint32 index, quint32 rec, bool asRaw) const
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return QVariant();
    }

    if ( !isColumnIndexValid(index)) {
        return QVariant();
    }
    if ( rec < recordCount()) {
        QVariant var = _datagrid[rec].cell(index);
        if ( !asRaw) {
            ColumnDefinition coldef = columndefinition(index);
            return coldef.datadef().domain<>()->impliedValue(var);
        }
        return var;
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg(name()),IssueObject::itWarning);
    return QVariant();
}

void  FlatTable::setCell(quint32 index, quint32 rec, const QVariant& var){
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( !isColumnIndexValid(index)) {
        return;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    _attributeDefinition[index].changed(true);

    if ( rec >= _datagrid.size()) {
        newRecord();
    }
    _datagrid[rec].cell(index, checkInput(var, index));
    _datagrid[rec].changed(true);

}

void FlatTable::setCell(const QString &col, quint32 rec, const QVariant &var)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    quint32 index = columnIndex(col);
    setCell(index, rec, var);

}

IlwisTypes FlatTable::ilwisType() const
{
    return itFLATTABLE;
}

IlwisObject *FlatTable::clone()
{
    if (initLoad()){
        FlatTable *tbl = new FlatTable();
        copyTo(tbl);
        tbl->_datagrid = _datagrid;
        for(auto& record : tbl->_datagrid){
            record.changed(false);
        }
        return tbl;
    }
    return 0;
}

bool FlatTable::canUse(const IlwisObject *obj, bool strict) const
{
    return true;
}

void FlatTable::copyTo(IlwisObject *obj){
    BaseTable::copyTo(obj);
}

bool FlatTable::initLoad(){
    if ( isDataLoaded()) {
        return true;
    }

    bool ok = BaseTable::initLoad();

    for(int i=0; ok && i < columnCount() && _datagrid.size() > 0; ++i){
        QVariant var = cell(i,0);
        if ( !var.isValid()) {
            initValuesColumn(columndefinition(i).name());
        }
    }
    return ok;
}
