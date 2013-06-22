#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"

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
        IlwisTypes ft = type & itPOINTCOVERAGE;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itPOINTCOVERAGE];
        ft = type & itSEGMENTCOVERAGE;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itSEGMENTCOVERAGE];
        ft = type & itPOLYGONCOVERAGE;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itPOLYGONCOVERAGE];
        ft = type & itGRIDCOVERAGE;
        if ( ft && _attTables[index].contains(ft))
            return _attTables[index][itGRIDCOVERAGE];
    }
    return ITable();
}

void Coverage::attributeTable(IlwisTypes type, const ITable &tbl, qint32 ind)
{
    if ( type & itCOVERAGE && ind < (qint32)_attTables.size()) {
        quint32 index = ind < 0 ? 0 : ind + 1;
        if ( index == _attTables.size())
            _attTables.push_back(AttributeTables());
        if ( (type & itPOINTCOVERAGE) != 0 )
            _attTables[index][itPOINTCOVERAGE] = tbl;
        if ( (type & itSEGMENTCOVERAGE) != 0 )
            _attTables[index][itSEGMENTCOVERAGE] = tbl;
        if ( (type & itPOLYGONCOVERAGE) != 0 )
            _attTables[index][itPOLYGONCOVERAGE] = tbl;
        if ( (type & itGRIDCOVERAGE) != 0 )
            _attTables[index][itGRIDCOVERAGE] = tbl;
    }
}

CoverageStatistics &Coverage::statistics()
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


