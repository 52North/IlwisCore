#ifndef ATTRIBUTEVISUALPROPERTIES_H
#define ATTRIBUTEVISUALPROPERTIES_H

#include "ilwiscoreui_global.h"

namespace Ilwis {

class Representation;
class Domain;
class RasterCoverage;

typedef IlwisData<Representation> IRepresentation;
typedef IlwisData<Domain> IDomain;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Geodrawer{

class ILWISCOREUISHARED_EXPORT VisualAttribute
{
public:
    VisualAttribute();
    VisualAttribute(const NumericRange& rng);
    VisualAttribute(const IDomain& dom, int colindex = iUNDEF, const Ilwis::IRepresentation &rpr = IRepresentation());
    VisualAttribute(const VisualAttribute& avp);

    IRepresentation representation() const;
    void representation(const IRepresentation& rpr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    NumericRange stretchRange() const;
    void stretchRange(const NumericRange& rng);
    QColor value2color(const QVariant& var) const;
    NumericRange actualRange() const;
    void actualRange(const NumericRange& rng);
    quint32 columnIndex() const;
    void setColumnIndex(quint32 index);

    const static QString LAYERATTRIBUTE;

    std::vector<QColor> colors(int size=256) const;
private:
    IRepresentation _representation;
    NumericRange _stretchRange;
    NumericRange _actualRange;
    IDomain _domain;
    IRasterCoverage _raster;
    quint32 _columnIndex=iUNDEF; //optional indicator which column/attribute is used for these properties
};
}
}

#endif // ATTRIBUTEVISUALPROPERTIES_H
