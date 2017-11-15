#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "attributedefinition.h"
#include "table.h"
#include "selectabletable.h"
#include "basetable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "tablemerger.h"
#include "logicalexpressionparser.h"
#include "tableselector.h"

using namespace Ilwis;

BaseTable::BaseTable() : SelectableTable(), _rows(0), _columns(0), _dataloaded(false)
{
}

BaseTable::BaseTable(const Resource& resource) : SelectableTable(resource), _rows(0), _columns(0),_dataloaded(false) {

}

BaseTable::~BaseTable()
{
}

quint32 BaseTable::recordCount() const
{
    return _rows;
}

quint32 BaseTable::columnCount() const
{
    return _columns;
}

void BaseTable::recordCount(quint32 r)
{
    _rows = r;
}

QString BaseTable::primaryKey() const {
    if (constConnector(IlwisObject::cmINPUT)->hasProperty("primaryKey"))
        return constConnector(IlwisObject::cmINPUT)->getProperty("primaryKey").toString();

    return "?";
}

bool BaseTable::createTable()
{
    if (!isValid()) {
        kernel()->issues()->log(TR("Not created, Table %1 already exists").arg(name()), IssueObject::itWarning);
        return false;
    }
    return true;
}

bool BaseTable::addColumn(const ColumnDefinition& def){

    if ( !def.isValid())
        return false;

    if ( isReadOnly())
        return false;
    changed(true);

    bool ok =_attributeDefinition.addColumn(def);
    _columns = std::max(_columns, _attributeDefinition.definitionCount());

    return ok;
}

bool BaseTable::addColumn(const QString &columnname, const IDomain& domain, const bool readonly)
{
    return addColumn(ColumnDefinition(columnname, domain,_columns, readonly));
}

bool BaseTable::addColumn(const QString &columnname, const QString &domainname, const bool readonly)
{
    if ( isReadOnly())
        return false;
    changed(true);

    bool ok = _attributeDefinition.addColumn(columnname, domainname);
    _columns = std::max(_columns, _attributeDefinition.definitionCount());

    return ok;
}

IlwisTypes BaseTable::ilwisType() const
{
    return itTABLE;
}

ColumnDefinition BaseTable::columndefinition(const QString &columnname) const
{
    return _attributeDefinition.columndefinition(columnname);
}

ColumnDefinition BaseTable::columndefinition(quint32 columnindex) const
{
    return _attributeDefinition.columndefinition(columnindex);
}

ColumnDefinition &BaseTable::columndefinitionRef(quint32 columnindex)
{
    return _attributeDefinition.columndefinitionRef(columnindex);
}

ColumnDefinition &BaseTable::columndefinitionRef(const QString &columnname)
{
    return _attributeDefinition.columndefinitionRef(columnname);
}

void BaseTable::columndefinition(const ColumnDefinition &coldef)
{
    _attributeDefinition.columndefinition(coldef);
}

bool BaseTable::prepare(const IOOptions &options)
{
    if (!IlwisObject::prepare(options))
        return false;

    return true;
}

bool BaseTable::isValid() const
{
    return true;
}

bool  BaseTable::initLoad() {
    if ( !this->isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg("table"));
        return false;
    }
    if (!_dataloaded) {
        _dataloaded = true; // to prevent other inits to pass through here
        if (!connector().isNull() && ! connector()->loadData(this)) {
            kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg("table", name()));
             _dataloaded = false;
            return false;
        }
    }
    return true;
}

void BaseTable::dataLoaded(bool yesno)
{
    _dataloaded = yesno;
}

void BaseTable::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    BaseTable *btable = static_cast<BaseTable *>(obj);
    btable->_columns = _columns;
    btable->_attributeDefinition = _attributeDefinition;
    btable->_dataloaded = _dataloaded;

    for(int colIndex = 0; colIndex < _columns; ++colIndex) {
        std::vector<QVariant> colvalues = column(colIndex);
        btable->column(colIndex, colvalues);
    }
    btable->_rows = _rows;
}

quint32 BaseTable::columnIndex(const QString &columnname) const
{
    return _attributeDefinition.columnIndex(columnname);
}

void BaseTable::columnCount(int cnt)
{
    if ( isReadOnly())
        return ;
    changed(true);

    if ( cnt >= 0)
        _columns = cnt;
}

bool BaseTable::merge(const IlwisObject *obj, int options)
{
    if (obj == 0 || ! hasType(obj->ilwisType(), itTABLE))
        return false;

    if ( id() == obj->id())
        return true;

    if ( isReadOnly())
        return false ;
    changed(true);

    ITable tblTarget(this);
    ITable tblSource(static_cast<Table *>(const_cast<IlwisObject *>(obj)));

    TableMerger merger;
    merger.copyColumns(tblSource, tblTarget, options);

    return true;
}

bool BaseTable::isDataLoaded() const
{
    return _dataloaded;
}

void BaseTable::adjustRange(int index) {

    ColumnDefinition& coldef = columndefinitionRef(index);
    if (!coldef.isValid())
        return;

    if( hasType(coldef.datadef().domain<>()->ilwisType(), itNUMERICDOMAIN)) {
        SPNumericRange rng = coldef.datadef().range<NumericRange>();
        std::vector<QVariant> values = column(coldef.id());
        if ( values.size() > 0 && !rng.isNull()) {
            double vmin=1e208, vmax=-1e208;
            bool hasfraction = true;
            for(const QVariant& var : values ){
                double v = var.toDouble();
                if ( !isNumericalUndef2(v, this))
                    vmin = std::min(vmin, v) ;
                v = var.toDouble();
                if (!isNumericalUndef2(v, this))                         {
                    vmax = std::max(vmax, v)    ;
                }
                hasfraction |= (v - (qint64)v != 0);

            }
            if ( vmin != 1e208 && vmax != -1e208) { //something has changed
                rng->min(vmin);
                rng->max(vmax);
                if (!hasfraction)
                    rng->resolution(1);
            }
        }
    } else if ( hasType(coldef.datadef().domain<>()->ilwisType(), itITEMDOMAIN)) {
        SPItemRange rng = coldef.datadef().range<ItemRange>();
        SPItemRange rngDomain = coldef.datadef().domain<>()->range<ItemRange>();
        std::vector<QVariant> values = column(coldef.id());
        if ( values.size() > 0 && !rng.isNull()) {
            rng->clear();
            for(auto qval : values) {
                quint32 id = qval.toUInt();
                SPDomainItem item = rngDomain->item(id);
                if ( !item.isNull()) {
                    rng->add(item->clone());
                }
            }
        }
    }
    coldef.changed(false);

}

QVariant BaseTable::checkInput(const QVariant& inputVar, quint32 columnIndex)  {
    return _attributeDefinition.checkInput(inputVar, columnIndex) ;
}


void BaseTable::initValuesColumn(const QString& colname){
    ColumnDefinition def = columndefinition(colname);
    if ( !def.isValid()){
        ERROR2(WARN_INVALID_OBJECT,TR("column definition"), name());
        return;
    }

    IlwisTypes valueType = def.datadef().domain<>()->valueType();
    std::vector<QVariant> col(recordCount());
    for(auto& var : col) {
        if ( hasType(valueType, itINTEGER | itDOMAINITEM))
            var = QVariant((int)iUNDEF);
        else if ( hasType(valueType, itDOUBLE | itFLOAT))
           var = QVariant(rUNDEF);
        else if ( hasType(valueType, itSTRING))
            var = QVariant(sUNDEF);
        else if ( hasType(valueType, itCOORDINATE))
            var.setValue(crdUNDEF);
    }
    column(def.name(),col);
}

void BaseTable::initRecord(std::vector<QVariant> &values) const
{
    values.resize(columnCount());
    for(int i=0; i < columnCount(); ++i) {
        const ColumnDefinition &coldef  = const_cast<BaseTable *>(this)->columndefinitionRef(i);
        if ( hasType(coldef.datadef().domain<>()->ilwisType(),itTEXTDOMAIN)) {
            values[i] = sUNDEF;
        }
        if ( hasType(coldef.datadef().domain<>()->ilwisType(),itITEMDOMAIN) ){
            values[i] = QVariant((int)iUNDEF);
        }
        if ( hasType(coldef.datadef().domain<>()->ilwisType(),itNUMERICDOMAIN)){
            values[i] = rUNDEF;
        }
    }
}

void BaseTable::removeRecord(quint32 rec)
{
    --_rows;
}






