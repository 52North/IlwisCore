#ifndef ILWISTYPES2_H
#define ILWISTYPES2_H

#include "kernel.h"

namespace Ilwis {
namespace Desktopclient {
class IlwisType : public QObject{
    Q_OBJECT

    Q_PROPERTY( quint64 itUnknown READ unknown CONSTANT)
    Q_PROPERTY( quint64 itPoint READ point CONSTANT)
    Q_PROPERTY( quint64 itLine READ line CONSTANT)
    Q_PROPERTY( quint64 itPolygon READ polygon CONSTANT)
    Q_PROPERTY( quint64 itGrid READ grid CONSTANT)
    Q_PROPERTY( quint64 itNumericDomain READ numericdomain CONSTANT)
    Q_PROPERTY( quint64 itItemDomain READ itemdomain CONSTANT)
    Q_PROPERTY( quint64 itCoordDomain READ coorddomain CONSTANT)
    Q_PROPERTY( quint64 itColorDomain READ colordomain CONSTANT)
    Q_PROPERTY( quint64 itTextdomain READ textdomain CONSTANT)
    Q_PROPERTY( quint64 itConventionalCoordsystem READ conventionalcoordsystem CONSTANT)
    Q_PROPERTY( quint64 itOrthoCoordsystem READ orthocoordsystem CONSTANT)
    Q_PROPERTY( quint64 itEllipsoid READ ellipsoid CONSTANT)
    Q_PROPERTY( quint64 itProjection READ projection CONSTANT)
    Q_PROPERTY( quint64 itAttributeTable READ attributetable CONSTANT)
    Q_PROPERTY( quint64 itFlatTable READ flattable CONSTANT)
    Q_PROPERTY( quint64 itGeoref READ georef CONSTANT)
    Q_PROPERTY( quint64 itEnvelope READ envelope CONSTANT)
    Q_PROPERTY( quint64 itOperationMetadata READ operationmetadata CONSTANT)
    Q_PROPERTY( quint64 itSingleOperation READ singleoperation CONSTANT)
    Q_PROPERTY( quint64 itWorkflow READ workflow CONSTANT)
    Q_PROPERTY( quint64 itGridSize READ gridsize CONSTANT)
    Q_PROPERTY( quint64 itCatalog READ catalog CONSTANT)
    Q_PROPERTY( quint64 itGeodeticDatum READ geodeticdatum CONSTANT)
    Q_PROPERTY( quint64 itBool READ boolean CONSTANT)
    Q_PROPERTY( quint64 itInt8 READ int8 CONSTANT)
    Q_PROPERTY( quint64 itUint8 READ uint8 CONSTANT)
    Q_PROPERTY( quint64 itUint16 READ uint16 CONSTANT)
    Q_PROPERTY( quint64 itInt16 READ int16 CONSTANT)
    Q_PROPERTY( quint64 itUint32 READ uint32 CONSTANT)
    Q_PROPERTY( quint64 itInt32 READ int32 CONSTANT)
    Q_PROPERTY( quint64 itUint64 READ uint64 CONSTANT)
    Q_PROPERTY( quint64 itInt64 READ int64 CONSTANT)
    Q_PROPERTY( quint64 itFloat READ real32 CONSTANT)
    Q_PROPERTY( quint64 itDouble READ real64 CONSTANT)
    Q_PROPERTY( quint64 itValuerange READ valuerange CONSTANT)
    Q_PROPERTY( quint64 itString READ string CONSTANT)
    Q_PROPERTY( quint64 itTime READ time CONSTANT)
    Q_PROPERTY( quint64 itCoordinate READ coordinate CONSTANT)
    Q_PROPERTY( quint64 itPixel READ pixel CONSTANT)
    Q_PROPERTY( quint64 itBinary READ binary CONSTANT)
    Q_PROPERTY( quint64 itFile READ file CONSTANT)
    Q_PROPERTY( quint64 itUrl READ url CONSTANT)
    Q_PROPERTY( quint64 itThematicItem READ thematicitem CONSTANT)
    Q_PROPERTY( quint64 itNamedItem READ nameditem CONSTANT)
    Q_PROPERTY( quint64 itIndexedItem READ indexeditem CONSTANT)
    Q_PROPERTY( quint64 itNumericItem READ numericitem CONSTANT)
    Q_PROPERTY( quint64 itTimeItem READ timeitem CONSTANT)
    Q_PROPERTY( quint64 itColor READ color CONSTANT)
public:
    quint64 unknown() const;
    quint64 point() const;
    quint64 line() const;
    quint64 polygon() const;
    quint64 grid() const;
    quint64 numericdomain() const;
    quint64 itemdomain() const;
    quint64 timedomain() const;
    quint64 coorddomain() const;
    quint64 colordomain() const;
    quint64 textdomain() const;
    quint64 conventionalcoordsystem() const;
    quint64 orthocoordsystem() const;
    quint64 ellipsoid() const;
    quint64 projection() const;
    quint64 flattable() const;
    quint64 georef() const;
    quint64 envelope() const;
    quint64 operationmetadata() const;
    quint64 singleoperation() const;
    quint64 workflow() const;
    quint64 gridsize() const;
    quint64 catalog() const;
    quint64 geodeticdatum() const;
    quint64 boolean() const;
    quint64 int8() const;
    quint64 uint8() const;
    quint64 uint16() const;
    quint64 int16() const;
    quint64 uint32() const;
    quint64 int32() const;
    quint64 uint64() const;
    quint64 int64() const;
    quint64 real32() const;
    quint64 real64() const;
    quint64 valuerange() const;
    quint64 string() const;
    quint64 time() const;
    quint64 coordinate() const;
    quint64 pixel() const;
    quint64 binary() const;
    quint64 file() const;
    quint64 url() const;
    quint64 thematicitem() const;
    quint64 nameditem() const;
    quint64 indexeditem() const;
    quint64 numericitem() const;
    quint64 timeitem() const;
    quint64 color() const;
    quint64 attributetable() const;
};
}
}

#endif
