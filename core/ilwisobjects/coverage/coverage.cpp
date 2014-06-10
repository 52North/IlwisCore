#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "domain.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "interval.h"
#include "itemrange.h"
#include "intervalrange.h"

using namespace Ilwis;

Coverage::Coverage()
{
}

Coverage::Coverage(const Resource &resource) : IlwisObject(resource)
{
    _attTableIndex.prepare();
    _attTableIndex->addColumn(FEATUREIDCOLUMN,"count");
    _attTableIndex->addColumn(TRACKINDEXCOLUMN,"count");
   IDomain dom("count");
   indexDomain(dom);
}

Coverage::~Coverage()
{
    _coordinateSystem.set(0);
}

bool Coverage::prepare( ) {

    bool ok = IlwisObject::prepare();

    return ok;
}

ICoordinateSystem Coverage::coordinateSystem() const
{
    return _coordinateSystem;
}

void Coverage::coordinateSystem(const ICoordinateSystem &csy)
{
    if ( isReadOnly())
        return;
    changed(true);

    _coordinateSystem = csy;
}

Envelope Coverage::envelope() const
{
    return _envelope;
}

void Coverage::envelope(const Envelope &bnds)
{
    if ( isReadOnly())
        return;
    changed(true);

    _envelope = bnds;
}

AttributeTable Coverage::attributeTable(AttributeType attType) const
{
    if ( attType == atCOVERAGE)
        return _attTable;
    return _attTableIndex;
}

AttributeTable& Coverage::attributeTable(AttributeType attType)
{
    if ( attType == atCOVERAGE)
        return _attTable;
    return _attTableIndex;
}

bool Coverage::hasAttributes(Coverage::AttributeType attType) const
{
    if ( attType == atCOVERAGE)
        return _attTable.isValid();
    return _attTableIndex.isValid();
}

void Coverage::attributeTable(const ITable& tbl, AttributeType attType)
{
    if ( isReadOnly())
        return;
    changed(true);
    if ( attType == atCOVERAGE)
        _attTable = tbl;
    else
        _attTableIndex = tbl;
}

NumericStatistics &Coverage::statistics(int  )
{
        return _statistics;
}

const IndexDefinition &Coverage::indexDefinition() const
{
    return _indexdefinition;
}

IndexDefinition &Coverage::indexDefinition()
{
    return _indexdefinition;
}

void Coverage::indexDomain(const IDomain& dom)
{
    _indexdefinition = {dom};
    if ( _attTableIndex->columnIndex(TRACKVALUECOLUMN) == iUNDEF)
        _attTableIndex->addColumn(TRACKVALUECOLUMN, dom);
    else
        _attTableIndex->initValuesColumn(TRACKVALUECOLUMN);
}

void Coverage::indexValues(const std::vector<QVariant>& values){
    int rec = 0;
    _attTableIndex->initValuesColumn(TRACKVALUECOLUMN);
    for(auto trackIndexValue : values){
        if ( indexDefinition().domain()->contains(trackIndexValue)){
            _attTableIndex->setCell(TRACKVALUECOLUMN, rec, trackIndexValue);
        }

    }
}


QVariant Coverage::value(const QString &colName, quint32 itemid, qint32 index)
{
    AttributeTable tbl = attributeTable(index != -1 ? atINDEX : atCOVERAGE);
    if (!tbl.isValid())
        return QVariant();
    ColumnDefinition coldef = tbl->columndefinition(colName);
    if ( !coldef.isValid())
        return QVariant();
    return coldef.datadef().domain<>()->impliedValue(itemid);
}

Resource Coverage::source(int mode) const
{
    Resource resource = IlwisObject::source(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        resource.addProperty("coordinatesystem", coordinateSystem()->id());
        resource.setExtendedType( resource.extendedType() | itCOORDSYSTEM);
    }
    return resource;
}

void Coverage::name(const QString &nam)
{
    if ( isReadOnly())
        return;
    changed(true);

    IlwisObject::name(nam);
    if ( _attTable.isValid()) {
        if ( _attTable->isAnonymous()) {
            _attTable->name(nam + "_attributes");
        }
    }
    if ( _attTableIndex.isValid()) {
        if (_attTableIndex->isAnonymous()) {
            _attTable->name(nam + "_indexattributes");
        }
    }
}

void Coverage::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    Coverage *cov = static_cast<Coverage *>(obj);
    cov->_coordinateSystem = _coordinateSystem;
    cov->_envelope = _envelope;
    cov->_attTable = _attTable;
    cov->_attTableIndex = _attTableIndex;
    cov->_statistics = _statistics;
    cov->_indexdefinition = _indexdefinition;
}


