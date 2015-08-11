#ifndef CONTINUOUSCOLORLOOKUP_H
#define CONTINUOUSCOLORLOOKUP_H

#include "colorlookup.h"

namespace Ilwis {

class Domain;
typedef IlwisData<Domain> IDomain;

class KERNELSHARED_EXPORT ContinuousColorLookup : public ColorLookUp
{
public:
        ContinuousColorLookup();
        ContinuousColorLookup(const QString& definition);
        ContinuousColorLookup(const IDomain&, const QString &rprCode="");
        QColor value2color(double value, const Ilwis::NumericRange &actualRange = NumericRange(), const Ilwis::NumericRange &stretchRange = NumericRange()) const;
        void addGroup(const NumericRange& range, const ContinuousColorRange& colorrange);
        void setColor(double value, const QColor& clr) ;
        ColorLookUp *clone() const;

private:
        std::vector<NumericRange> _groups;
        std::vector<ContinuousColorRange> _colorranges;
        NumericRange _numericRange;
        double _step = 0;
        bool _linear = true;

        void fromDefinition(const QString& def);

};
}

#endif // CONTINUOUSCOLORLOOKUP_H
