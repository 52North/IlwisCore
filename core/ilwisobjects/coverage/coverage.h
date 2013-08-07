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

    ICoordinateSystem coordinateSystem() const;
    void setCoordinateSystem(const ICoordinateSystem& csy);
    Box3D<double> envelope() const;
    void envelope(const Box3D<double> &bnds);

    ITable attributeTable(IlwisTypes type, qint32 ind=-1) const ;
    void attributeTable(IlwisTypes type, const ITable& tbl, qint32 index=-1 );
    NumericStatistics& statistics();
    const DataDefinition& datadef() const;
    DataDefinition& datadef();
    QVariant value(const QString& colName, quint32 itemid, IlwisTypes type=itFEATURECOVERAGE, qint32 index = -1);

protected:
    void copyTo(IlwisObject *obj) ;
private:

    ICoordinateSystem _coordinateSystem;
    Box3D<double> _envelope;
    std::vector<AttributeTables> _attTables;
    NumericStatistics _statistics;
    DataDefinition _datadef;

};

typedef IlwisData<Coverage> ICoverage;


}





#endif // BASELAYER_H
