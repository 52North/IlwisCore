#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "valuedefiner.h"
#include "connectorinterface.h"
#include "columndefinition.h"
#include "table.h"
#include "coordinatesystem.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "resource.h"

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

Box2D<double> Coverage::envelope() const
{
    return _envelope;
}

void Coverage::setEnvelope(const Box2D<double> &bnds)
{
    _envelope = bnds;
}

ITable Coverage::attributeTable(IlwisTypes type) const
{
    IlwisTypes ft = type & itPOINTCOVERAGE;
    if ( ft && _attributeTables.contains(ft)) {
        return _attributeTables[itPOINTCOVERAGE];
    } else {
        IlwisTypes ft = type & itSEGMENTCOVERAGE;
        if ( ft && _attributeTables.contains(ft)) {
            return _attributeTables[itSEGMENTCOVERAGE];
        } else {
            IlwisTypes ft = type & itPOLYGONCOVERAGE;
            if ( ft && _attributeTables.contains(ft)) {
                return _attributeTables[itPOLYGONCOVERAGE];
            } else {
                IlwisTypes ft = type & itGRIDCOVERAGE;
                if ( ft && _attributeTables.contains(ft)) {
                    return _attributeTables[itGRIDCOVERAGE];
                }
            }
        }
    }
    return ITable();
}

void Coverage::attributeTable(IlwisTypes type, const ITable &tbl)
{
    if ( type & itCOVERAGE) {
        IlwisTypes ft = type & itPOINTCOVERAGE;
        if ( ft) {
            _attributeTables[ft] = tbl;
        }
        ft = type & itSEGMENTCOVERAGE;
        if ( ft) {
            _attributeTables[ft] = tbl;
        }
        ft = type & itPOLYGONCOVERAGE;
        if ( ft) {
            _attributeTables[ft] = tbl;
        }
        ft = type & itGRIDCOVERAGE;
        if ( ft) {
            _attributeTables[ft] = tbl;
        }
    }
}

CoverageStatistics &Coverage::statistics()
{
    return _statistics;
}




