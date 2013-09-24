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
#include "commandhandler.h"

namespace Ilwis {

class Table;
class ItemRange;

typedef IlwisData<Table> ITable;
typedef QHash<IlwisTypes, ITable> AttributeTables;

class KERNELSHARED_EXPORT Coverage : public IlwisObject
{

public:
    Coverage();
    Coverage(const Resource& source);
    ~Coverage();

    ICoordinateSystem coordinateSystem() const;
    void setCoordinateSystem(const ICoordinateSystem& csy);
    Box3D<double> envelope() const;
    void envelope(const Box3D<double> &bnds);

    ITable attributeTable(IlwisTypes type, qint32 ind=-1) const ;
    void attributeTable(IlwisTypes type, const ITable& tbl, qint32 layerIndex=-1 );
    NumericStatistics& statistics();
    const DataDefinition& datadefIndex() const;
    DataDefinition& datadefIndex();
    QVariant value(const QString& colName, quint32 itemid, IlwisTypes type=itFEATURE, qint32 layerIndex = -1);
    Resource source(int mode=cmINPUT) const;
    double layerIndex(const QString& value);
    void setLayerIndexes(const ItemRange &items);

protected:
    void copyTo(IlwisObject *obj) ;
private:

    ICoordinateSystem _coordinateSystem;
    Box3D<double> _envelope;
    std::vector<AttributeTables> _attTables;
    NumericStatistics _statistics;
    DataDefinition _indexdefinition;
    std::vector<quint32> _indexValues;

};

typedef IlwisData<Coverage> ICoverage;


}





#endif // BASELAYER_H
