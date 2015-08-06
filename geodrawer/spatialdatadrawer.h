#ifndef SPATIALDATADRAWER_H
#define SPATIALDATADRAWER_H

#include "complexdrawer.h"

namespace Ilwis {


class Coverage;
class ColumnDefinition;

typedef IlwisData<Coverage> ICoverage;

namespace Geodrawer{

class VisualAttribute;

class SpatialDataDrawer : public ComplexDrawer
{
public:
    SpatialDataDrawer(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    std::vector<double> numericAttributeValues(const QString& attribute) const;
    virtual ICoverage coverage() const;
    Envelope envelope() const;
    void envelope(const Envelope& env);
    VisualAttribute visualProperty(const QString& attrName) const;
    void visualProperty(const QString &attrName, const VisualAttribute& properties );
    virtual void coverage(const ICoverage& cov);

    void setAttribute(const QString &key, const QVariant &attrib);
    VisualAttribute createVisualProperty(const Ilwis::ColumnDefinition &coldef, int index, const IRepresentation &rpr = IRepresentation());
    QVariant attribute(const QString &key) const;
    std::vector<QVariant> attributes(const QString &keys) const;

protected:
    bool prepare(PreparationType prepType, const IOOptions& options);
    bool isVisualProperty(const QString& attName) const;

    std::map<QString, VisualAttribute> _visualProperties;
private:

    ICoverage _coverage;
    Envelope _envelope;
    VisualAttribute vPropertyRasterCoverage(const QString &attrName) const;
    VisualAttribute vPropertyFeatureCoverage(const QString &attrName) const;
};
}
}

#endif // SPATIALDATADRAWER_H
