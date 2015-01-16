#ifndef SPATIALDATADRAWER_H
#define SPATIALDATADRAWER_H

#include "complexdrawer.h"

namespace Ilwis {


class Coverage;
typedef IlwisData<Coverage> ICoverage;

namespace Geodrawer{

class AttributeVisualProperties;

class SpatialDataDrawer : public ComplexDrawer
{
public:
    SpatialDataDrawer(const QString& name,DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    std::vector<double> numericAttributeValues(const QString& attribute) const;
    virtual ICoverage coverage() const;
    Envelope envelope() const;
    void envelope(const Envelope& env);
    AttributeVisualProperties dataAttribute(const QString& attrName) const;
    void dataAttribute(const QString &attrName, const AttributeVisualProperties& properties );
    virtual void coverage(const ICoverage& cov);

protected:
private:
    std::map<QString, AttributeVisualProperties> _visualProperties;
    ICoverage _coverage;
    Envelope _envelope;
};
}
}

#endif // SPATIALDATADRAWER_H
