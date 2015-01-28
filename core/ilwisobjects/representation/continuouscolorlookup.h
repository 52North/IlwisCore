#ifndef CONTINUOUSCOLORLOOKUP_H
#define CONTINUOUSCOLORLOOKUP_H

#include "colorlookup.h"

namespace Ilwis {

class DataDefinition;

class KERNELSHARED_EXPORT ContinuousColorLookup : public ColorLookUp
{
public:
        ContinuousColorLookup(const QString& definition);
        QColor value2color(double value, const Ilwis::NumericRange &actualRange = NumericRange(), const Ilwis::NumericRange &stretchRange = NumericRange()) const;
        void addGroup(const NumericRange& range, const ContinuousColorRange& colorrange);

private:
        std::vector<NumericRange> _groups;
        std::vector<ContinuousColorRange> _colorranges;
        NumericRange _numericRange;
        double _step = 0;
        bool _linear = true;

};
}

#endif // CONTINUOUSCOLORLOOKUP_H
