#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericitem.h"
#include "itemrange.h"
#include "numericitemrange.h"

using namespace Ilwis;

Coverage::Coverage()
{
}

Coverage::Coverage(const Resource &resource) : IlwisObject(resource)
{

}

Coverage::~Coverage()
{
}

ICoordinateSystem Coverage::coordinateSystem() const
{
    return _coordinateSystem;
}

void Coverage::setCoordinateSystem(const ICoordinateSystem &csy)
{
    if ( isReadOnly())
        return;
    changed(true);

    _coordinateSystem = csy;
}

Box3D<double> Coverage::envelope() const
{
    return _envelope;
}

void Coverage::envelope(const Box3D<double> &bnds)
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

void Coverage::attributeTable(const ITable& tbl, AttributeType attType)
{
    if ( isReadOnly())
        return;
    changed(true);
    if ( attType == atCOVERAGE)
        _attTable = tbl;
    _attTableIndex = tbl;
}

NumericStatistics &Coverage::statistics()
{
    return _statistics;
}

const DataDefinition &Coverage::datadefIndex() const
{
    return _indexdefinition;
}

DataDefinition &Coverage::datadefIndex()
{
    return _indexdefinition;
}


QVariant Coverage::value(const QString &colName, quint32 itemid, qint32 index)
{
    AttributeTable tbl = attributeTable(index != -1 ? atINDEX : atCOVERAGE);
    if (!tbl.isValid())
        return QVariant();
    ColumnDefinition coldef = tbl->columndefinition(colName);
    if ( !coldef.isValid())
        return QVariant();
    return coldef.datadef().domain()->value(itemid);
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

double Coverage::layerIndex(const QString &value)
{
    if (! _indexdefinition.range()->contains(value))
        return rUNDEF;

    bool isContinuous = _indexdefinition.range()->isContinuous();
    if ( hasType(_indexdefinition.domain()->valueType(), itNUMBER )) {
        double v = value.toDouble();
        if ( isContinuous )
            return v;
        return (int)v;
    }
    if ( hasType(_indexdefinition.domain()->ilwisType(), itITEMDOMAIN)) {

        SPItemRange rng = _indexdefinition.domain()->range2range<ItemRange>();
        if ( !rng->isValid())
            return rUNDEF;
        IlwisTypes valueType = _indexdefinition.domain()->valueType();
        if ( hasType(valueType, itNUMERICITEM )) {
            SPNumericItemRange rng = _indexdefinition.domain()->range2range<NumericItemRange>();
            return rng->index(value.toDouble());
        }

        SPDomainItem domItem = rng->item(value);
        int idx = 0;
        for(quint32 rawAxis : _indexValues) {
             if ( rawAxis == domItem->raw())
                return idx;
             ++idx;
        }
    }
    return rUNDEF;
}

void Coverage::setLayerIndexes(const ItemRange &items)
{
    if ( isReadOnly())
        return;
    changed(true);

    _indexValues.resize(items.count());
    for(int i=0; i < items.count(); ++i) {
        SPDomainItem ditem = items.itemByOrder(i);
        _indexValues[i] = ditem.isNull() ? iUNDEF : ditem->raw();
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


