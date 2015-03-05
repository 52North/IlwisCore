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
    SpatialDataDrawer(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    std::vector<double> numericAttributeValues(const QString& attribute) const;
    virtual ICoverage coverage() const;
    Envelope envelope() const;
    void envelope(const Envelope& env);
    AttributeVisualProperties visualAttribute(const QString& attrName) const;
    void visualAttribute(const QString &attrName, const AttributeVisualProperties& properties );
    virtual void coverage(const ICoverage& cov);

    void setAttribute(const QString &attrName, const QVariant &attrib);
    QVariant attribute(const QString &key) const;
    std::vector<QVariant> attributes(const QString &keys) const;


protected:
    bool prepare(PreparationType prepType, const IOOptions& options);
    bool isVisualAttribute(const QString& attName) const;

    std::map<QString, AttributeVisualProperties> _visualProperties;
private:

    ICoverage _coverage;
    Envelope _envelope;
};
}
}

#endif // SPATIALDATADRAWER_H
