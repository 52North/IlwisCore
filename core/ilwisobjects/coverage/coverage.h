#ifndef BASELAYER_H
#define BASELAYER_H

#include "Kernel_global.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "connectorinterface.h"
#include "containerstatistics.h"

namespace Ilwis {

class Table;
typedef IlwisData<Table> ITable;
typedef QHash<IlwisTypes, ITable> AttributeTables;

class KERNELSHARED_EXPORT Coverage : public IlwisObject
{
public:
    Coverage();
    Coverage(const Resource& res);
    ~Coverage();

   // virtual QVector<double> value(const Coordinate& c) = 0;

    ICoordinateSystem coordinateSystem() const;
    void setCoordinateSystem(const ICoordinateSystem& csy);
    Box2D<double> envelope() const;
    void setEnvelope(const Box2D<double>& bnds);

    ITable attributeTable(IlwisTypes type, qint32 ind=-1) const ;
    void attributeTable(IlwisTypes type, const ITable& tbl, qint32 index=-1 );
    CoverageStatistics& statistics();
    const DataDefinition& datadef() const;
    DataDefinition& datadef();

private:
    ICoordinateSystem _coordinateSystem;
    Box2D<double> _envelope;
    std::vector<AttributeTables> _attTables;
    CoverageStatistics _statistics;
    DataDefinition _datadef;

};

typedef IlwisData<Coverage> ICoverage;


}



#endif // BASELAYER_H
