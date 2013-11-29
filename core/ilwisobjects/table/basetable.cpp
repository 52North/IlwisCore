#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "angle.h"
#include "point.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
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

    if ( _columnDefinitionsByName.contains(def.name())) {
        kernel()->issues()->log(TR("Adding duplicate column %1").arg(name()),IssueObject::itWarning);
        return false;
    }
    ColumnDefinition coldef = def;
    coldef.columnindex(_columnDefinitionsByIndex.size());
    _columnDefinitionsByName[coldef.name()] = coldef;
    _columnDefinitionsByIndex[coldef.columnindex()] = _columnDefinitionsByName[coldef.name()];
    _columns = _columnDefinitionsByName.size();

    return true;
}

bool BaseTable::addColumn(const QString &name, const IDomain& domain)
{
    if ( isReadOnly())
        return false;
    changed(true);

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
    if ( iter != _columnDefinitionsByName.end()) {
        if (iter.value().isChanged()) {
            int index = columnIndex(nme);
            const_cast<BaseTable *>(this)->adjustRange(index);
        }
       return iter.value();
    }
    return ColumnDefinition();
}

ColumnDefinition BaseTable::columndefinition(quint32 index) const
{
    auto iter = _columnDefinitionsByIndex.find(index);
    if ( iter != _columnDefinitionsByIndex.end()) {
        if (iter.value().isChanged()) {
            const_cast<BaseTable *>(this)->adjustRange(index);
        }
        return iter.value();
    }
    return ColumnDefinition();
}

ColumnDefinition &BaseTable::columndefinition(quint32 index)
{
    if ( _columnDefinitionsByIndex[index].isChanged()) {
        adjustRange(index);
    }
    return _columnDefinitionsByIndex[index];
}

void BaseTable::columndefinition(const ColumnDefinition &coldef)
{
    if ( coldef.id() >=  _columnDefinitionsByIndex.size())     {
        addColumn(coldef.name(), coldef.datadef().domain());
    } else {
        auto iter1 = _columnDefinitionsByIndex.find(coldef.id());
        auto iter2 = _columnDefinitionsByName.find(coldef.name());
        if ( iter1 != _columnDefinitionsByIndex.end()) {
            _columnDefinitionsByName.remove(iter1.value().name());
            _columnDefinitionsByIndex.remove(coldef.id());

        }else if ( iter2 != _columnDefinitionsByName.end()) {
            _columnDefinitionsByIndex.remove(iter2.value().id());
            _columnDefinitionsByName.remove(coldef.name());
        }
        _columnDefinitionsByName[coldef.name()] = coldef;
        _columnDefinitionsByIndex[coldef.id()] = coldef;
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
    return _rows != 0 && _columns != 0;
}

bool  BaseTable::initLoad() {
    if ( !this->isValid()) {
        kernel()->issues()->log(TR(ERR_NO_INITIALIZED_1).arg("table"));
        return false;
    }
    if (!_dataloaded) {
        _dataloaded = true; // to prevent other inits to pass through here
        if (!connector().isNull() && ! connector()->loadBinaryData(this)) {
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
    return iter.value().id();
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

    if( hasType(coldef.datadef().domain()->ilwisType(), itNUMERICDOMAIN)) {
        SPNumericRange rng = coldef.datadef().range<NumericRange>();
        std::vector<QVariant> values = column(coldef.id());
        if ( values.size() > 0 && !rng.isNull()) {
            double vmin=1e208, vmax=-1e208;
            for(const QVariant& var : values ){
                double v = var.toDouble();
                if ( !isNumericalUndef(v))
                    vmin = std::min(vmin, v) ;
                v = var.toDouble();
                if (!isNumericalUndef(v))                         {
                    vmax = std::max(vmax, v)    ;
                }

            }
            if ( vmin != 1e208 && vmax != -1e208) { //something has changed
                rng->min(vmin);
                rng->max(vmax);
                _columnDefinitionsByName[coldef.name()] = coldef;
            }
        }
    } else if ( hasType(coldef.datadef().domain()->ilwisType(), itITEMDOMAIN)) {
        SPItemRange rng = coldef.datadef().range<ItemRange>();
        SPItemRange rngDomain = coldef.datadef().domain()->range2range<ItemRange>();
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
            _columnDefinitionsByName[coldef.name()] = coldef;
        }
    }
    coldef.changed(false);

}

QVariant BaseTable::checkInput(const QVariant& inputVar, quint32 columnIndex)  {
    QVariant actualval= inputVar;
    if ( QString(inputVar.typeName()) == "QString"){
        QString txt = inputVar.toString();
        ColumnDefinition& coldef = columndefinition(columnIndex);
        if ( !coldef.isValid())
            return QVariant();

        if ( hasType(coldef.datadef().domain()->ilwisType(),itTEXTDOMAIN)) {
            return actualval;
        }
        if ( hasType(coldef.datadef().domain()->ilwisType(),itITEMDOMAIN) && txt == sUNDEF){
            return QVariant((int)iUNDEF);
        }
        if ( hasType(coldef.datadef().domain()->ilwisType(),itNUMERICDOMAIN) && txt == sUNDEF){
            return rUNDEF;
        }
        bool ok;
        double v = inputVar.toDouble(&ok);
        if ( ok ){
            actualval = v;
        } else {
        SPItemRange rng1 = coldef.datadef().domain()->range2range<ItemRange>();
        SPItemRange rng2 = coldef.datadef().range<ItemRange>();

        SPDomainItem item = rng1->item(inputVar.toString());
        if ( item.isNull()){
            WARN2(WARN_INVALID_OBJECT,"domain item "+ inputVar.toString(), "column");
            return QVariant((int)iUNDEF);
        }
        if ( !rng2->contains(item->name())){
            rng2->add(item->clone());
        }
        actualval = item->raw();
        }

    }
    return actualval;
}

void BaseTable::initValuesColumn(const ColumnDefinition& def){
    if ( !def.isValid()){
        ERROR2(WARN_INVALID_OBJECT,TR("column definition"), name());
        return;
    }

    IlwisTypes valueType = def.datadef().domain()->valueType();
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






