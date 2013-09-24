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
    _coordinateSystem = csy;
}

Box3D<double> Coverage::envelope() const
{
    return _envelope;
}

void Coverage::envelope(const Box3D<double> &bnds)
{
    _envelope = bnds;
}

ITable Coverage::attributeTable(IlwisTypes type, qint32 ind) const
{
    int index = ind < 0 ? 0 : ind + 1;
    if ( index < (qint32)_attTables.size()){
        IlwisTypes ft = type & itPOINT;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itPOINT];
        ft = type & itLINE;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itLINE];
        ft = type & itPOLYGON;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itPOLYGON];
        ft = type & itRASTER;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itRASTER];
    }
    return ITable();
}

void Coverage::attributeTable(IlwisTypes type, const ITable &tbl, qint32 ind)
{
    if ( ind < (qint32)_attTables.size()) {
        quint32 index = ind < 0 ? 0 : ind + 1;
        if ( index == _attTables.size())
            _attTables.push_back(AttributeTables());
        if ( (type & itPOINT) != 0 )
            _attTables[index][itPOINT] = tbl;
        if ( (type & itLINE) != 0 )
            _attTables[index][itLINE] = tbl;
        if ( (type & itPOLYGON) != 0 )
            _attTables[index][itPOLYGON] = tbl;
        if ( (type & itRASTER) != 0 )
            _attTables[index][itRASTER] = tbl;
    }
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


QVariant Coverage::value(const QString &colName, quint32 itemid, IlwisTypes type, qint32 index)
{
    ITable tbl = attributeTable(type, index);
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
        resource.setExtendedType( resource.extendedType() | itDOMAIN | itCOORDSYSTEM);
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

        SPItemRange rng = _indexdefinition.domain()->range<ItemRange>();
        if ( !rng->isValid())
            return rUNDEF;
        IlwisTypes valueType = _indexdefinition.domain()->valueType();
        if ( hasType(valueType, itNUMERICITEM )) {
            SPNumericItemRange rng = _indexdefinition.domain()->range<NumericItemRange>();
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
    cov->_attTables = _attTables;
    cov->_statistics = _statistics;
    cov->_indexdefinition = _indexdefinition;
}


