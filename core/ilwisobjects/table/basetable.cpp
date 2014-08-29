#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "table.h"
#include "basetable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "tablemerger.h"

using namespace Ilwis;

BaseTable::BaseTable() : Table(), _rows(0), _columns(0), _dataloaded(false)
{
}

BaseTable::BaseTable(const Resource& resource) : Table(resource), _rows(0), _columns(0),_dataloaded(false) {

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

    if ( isReadOnly())
        return false;
    changed(true);

    if ( _columnDefinitionsByName.find(def.name()) != _columnDefinitionsByName.end()) { // no duplicates
        return false;
    }
    ColumnDefinition coldef = def;
    coldef.columnindex(_columnDefinitionsByIndex.size());
    _columnDefinitionsByName[coldef.name()] = _columns;
    _columnDefinitionsByIndex.push_back(coldef);
    _columns = _columnDefinitionsByName.size();

    return true;
}

bool BaseTable::addColumn(const QString &name, const IDomain& domain)
{
    return addColumn(ColumnDefinition(name, domain,_columns));
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
    if ( iter != _columnDefinitionsByName.end()) {
       ColumnDefinition coldef = _columnDefinitionsByIndex[(*iter).second];
       return coldef;
    }
    return ColumnDefinition();
}

ColumnDefinition BaseTable::columndefinition(quint32 index) const
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    return ColumnDefinition();
}

ColumnDefinition &BaseTable::columndefinitionRef(quint32 index)
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    throw ErrorObject(TR(QString("Invalid column index for %1 used").arg(name())));
}

ColumnDefinition &BaseTable::columndefinitionRef(const QString &nme)
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter != _columnDefinitionsByName.end()) {
       ColumnDefinition& coldef = _columnDefinitionsByIndex[(*iter).second];
       return coldef;
    }
    throw ErrorObject(TR(QString("Invalid column name %1 used").arg(name())));
}

void BaseTable::columndefinition(const ColumnDefinition &coldef)
{
    if ( coldef.id() >=  _columnDefinitionsByIndex.size())     {
        addColumn(coldef.name(), coldef.datadef().domain<>());
    } else {
        auto iter1 = _columnDefinitionsByName.find(coldef.name());
        if ( iter1 != _columnDefinitionsByName.end()) {
            ColumnDefinition cdef = coldef;
            cdef.columnindex((*iter1).second);
            _columnDefinitionsByIndex[cdef.columnindex()] = cdef;
            (*iter1).second = cdef.columnindex();

        }else {
            addColumn(coldef);
        }
    }
}


bool BaseTable::prepare()
{
    if (!IlwisObject::prepare())
        return false;

    return true;
}

bool BaseTable::isValid() const
{
    return _columns != 0;
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
    btable->_columnDefinitionsByIndex = _columnDefinitionsByIndex;
    btable->_columnDefinitionsByName = _columnDefinitionsByName;
    btable->_dataloaded = _dataloaded;

    for(const auto& def : _columnDefinitionsByIndex) {
        std::vector<QVariant> colvalues = column(def.name());
        btable->column(def.name(), colvalues);
    }
    btable->_rows = _rows;
}

quint32 BaseTable::columnIndex(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter == _columnDefinitionsByName.end()) {
        //kernel()->issues()->log(TR(ERR_COLUMN_MISSING_2).arg(nme).arg(name()),IssueObject::itWarning);
        return iUNDEF;
    }
    return (*iter).second;
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

    ColumnDefinition& coldef = _columnDefinitionsByIndex[index];
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
                if ( !isNumericalUndef(v))
                    vmin = std::min(vmin, v) ;
                v = var.toDouble();
                if (!isNumericalUndef(v))                         {
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
    QVariant actualval= inputVar;
    ColumnDefinition& coldef = columndefinitionRef(columnIndex);
    QString typenm = inputVar.typeName();
    IlwisTypes domtype = coldef.datadef().domain<>()->ilwisType();
    IlwisTypes valueType = coldef.datadef().domain<>()->valueType();
    IDomain dm = coldef.datadef().domain<>();
    if ( domtype == itITEMDOMAIN){
        if ( inputVar == sUNDEF){
            return QVariant((int)iUNDEF);
        } else if ( typenm == "QString"){
            actualval = dm->impliedValue(inputVar);
            SPItemRange rng1 = dm->range<ItemRange>();
            SPItemRange rng2 = coldef.datadef().range<ItemRange>();

            SPDomainItem item = rng1->item(inputVar.toString());
            if ( item.isNull()){
                return QVariant((int)iUNDEF);
            }
            if ( !rng2->contains(item->name())){
                rng2->add(item->clone());
            }
            actualval = item->raw();
        }

    }else if ( domtype == itNUMERICDOMAIN){
        if (typenm == "QString")
            actualval =  dm->impliedValue(inputVar);
        if ( hasType(valueType,itDATETIME) && actualval.value<Ilwis::Time>() == tUNDEF)
            return actualval;
        else if (isNumericalUndef(actualval.toDouble()))
            return rUNDEF;
        if (! dm->contains(actualval))
            actualval = rUNDEF;
        else {
            SPNumericRange rng = coldef.datadef().range<NumericRange>();
            if ( !rng.isNull()){
                if ( hasType(valueType,itDATETIME))
                    rng->add(actualval.value<Ilwis::Time>());
                else
                    rng->add(actualval.toDouble());
            }
        }
    } else if ( domtype == itTEXTDOMAIN){
        return dm->impliedValue(inputVar);
    }
    return actualval;
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






