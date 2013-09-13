#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "domain.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "numericitem.h"
#include "itemrange.h"
#include "numericitemrange.h"

using namespace Ilwis;

Coverage::Coverage()
{
}

Coverage::Coverage(const Resource &res) : IlwisObject(res)
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
        ft = type & itGRID;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itGRID];
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
        if ( (type & itGRID) != 0 )
            _attTables[index][itGRID] = tbl;
    }
}

NumericStatistics &Coverage::statistics()
{
    return _statistics;
}

const DataDefinition &Coverage::datadef() const
{
    return _datadef;
}

DataDefinition &Coverage::datadef()
{
    return _datadef;
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

Resource Coverage::resource(int mode) const
{
    Resource res = IlwisObject::resource(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        res.addProperty("domain", datadef().domain()->id());
        res.addProperty("coordinatesystem", coordinateSystem()->id());
        res.setExtendedType( res.extendedType() | itDOMAIN | itCOORDSYSTEM);
    }
    return res;
}

double Coverage::index(const QString &value)
{
    DataDefinition::DomainAxis axis = DataDefinition::daINDEX;
    if (! datadef().range(axis)->contains(value))
        return rUNDEF;

    bool isContinuous = datadef().range(axis)->isContinuous();
    if ( hasType(datadef().domain(axis)->valueType(), itNUMBER )) {
        double v = value.toDouble();
        if ( isContinuous )
            return v;
        return (int)v;
    }
    if ( hasType(datadef().domain(axis)->ilwisType(), itITEMDOMAIN)) {

        SPItemRange rng = datadef().domain(axis)->range<ItemRange>();
        if ( !rng->isValid())
            return rUNDEF;
        IlwisTypes valueType = datadef().domain(axis)->valueType();
        if ( hasType(valueType, itNUMERICITEM )) {
            SPNumericItemRange rng = datadef().domain(axis)->range<NumericItemRange>();
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

bool Coverage::setIndexes(const ItemRange &items)
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
    cov->_datadef = _datadef;

}


