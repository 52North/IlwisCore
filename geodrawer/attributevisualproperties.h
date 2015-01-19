#ifndef ATTRIBUTEVISUALPROPERTIES_H
#define ATTRIBUTEVISUALPROPERTIES_H

namespace Ilwis {

class Representation;
class Domain;

typedef IlwisData<Representation> IRepresentation;
typedef IlwisData<Domain> IDomain;

namespace Geodrawer{

class AttributeVisualProperties
{
public:
    AttributeVisualProperties();
    AttributeVisualProperties(const IDomain& dom);
    AttributeVisualProperties(const AttributeVisualProperties& avp);

    IRepresentation representation() const;
    void representation(const IRepresentation& rpr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    NumericRange stretchRange() const;
    void stretchRange(const NumericRange& rng);
    QColor value2color(const QVariant& var);
    NumericRange actualRange() const;
    void actualRange(const NumericRange& rng);


private:
    IRepresentation _representation;
    NumericRange _stretchRange;
    NumericRange _actualRange;
    IDomain _domain;
};
}
}

#endif // ATTRIBUTEVISUALPROPERTIES_H
