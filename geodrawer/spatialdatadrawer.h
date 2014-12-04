#ifndef SPATIALDATADRAWER_H
#define SPATIALDATADRAWER_H

#include "complexdrawer.h"

namespace Ilwis {

class Coverage;
typedef IlwisData<Coverage> ICoverage;

namespace Geodrawer{

class SpatialDataDrawer : public ComplexDrawer
{
public:
    SpatialDataDrawer(const QString& name,DrawerInterface* parentDrawer, RootDrawer *rootdrawer);
    virtual void setCoverage(const ICoverage& coverage);

    std::vector<double> numericAttributeValues(const QString& attribute) const;
    NumericRange stretchRange();
    void stretchRange(const NumericRange& numrange);

    bool isStretched() const;
    void stretched(bool yesno);

    ICoverage coverage() const;
    void coverage(const ICoverage& cov);
    Envelope envelope() const;
    void envelope(const Envelope& env);

private:
    ICoverage _coverage;
    Envelope _envelope;
    NumericRange _stretchRange;
    bool _stretched = false;

};
}
}

#endif // SPATIALDATADRAWER_H
