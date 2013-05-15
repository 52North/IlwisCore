#ifndef BASELAYER_H
#define BASELAYER_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT Coverage : public IlwisObject, public ValueDefiner
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

    ITable attributeTable(IlwisTypes type) const ;
    void attributeTable(IlwisTypes type, const ITable& tbl);
    CoverageStatistics& statistics();

private:
    ICoordinateSystem _coordinateSystem;
    Box2D<double> _envelope;
    QHash<IlwisTypes, ITable> _attributeTables;
    CoverageStatistics _statistics;

};

typedef IlwisData<Coverage> ICoverage;
}



#endif // BASELAYER_H
