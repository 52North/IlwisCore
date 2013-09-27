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
typedef ITable AttributeTable;

class KERNELSHARED_EXPORT Coverage : public IlwisObject
{

public:
    enum AttributeType{atCOVERAGE, atINDEX};
    Coverage();
    Coverage(const Resource& source);
    ~Coverage();

    ICoordinateSystem coordinateSystem() const;
    void setCoordinateSystem(const ICoordinateSystem& csy);
    Box3D<double> envelope() const;
    void envelope(const Box3D<double> &bnds);

    AttributeTable attributeTable(AttributeType attType=atCOVERAGE) const ;
    void attributeTable(const ITable& tbl, AttributeType attType=atCOVERAGE );
    NumericStatistics& statistics();
    const DataDefinition& datadefIndex() const;
    DataDefinition& datadefIndex();
    QVariant value(const QString& colName, quint32 itemid, qint32 layerIndex = -1);
    Resource source(int mode=cmINPUT) const;
    double layerIndex(const QString& value);
    void setLayerIndexes(const ItemRange &items);

protected:
    void copyTo(IlwisObject *obj) ;
private:

    ICoordinateSystem _coordinateSystem;
    Box3D<double> _envelope;
    AttributeTable _attTable;
    AttributeTable _attTableIndex;
    NumericStatistics _statistics;
    DataDefinition _indexdefinition;
    std::vector<quint32> _indexValues;

};

typedef IlwisData<Coverage> ICoverage;


}





#endif // BASELAYER_H
