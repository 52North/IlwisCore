#ifndef ILWIS3RANGE_H
#define ILWIS3RANGE_H

namespace Ilwis {
namespace Ilwis3{

class Ilwis3Range
{

public:
    Ilwis3Range();
    static Range * findRange(const ODF &odf, const QString &section) ;
    static bool minMax2minMax(const Ilwis::ODF &odf, const QString &minmax, double &vmin, double &vmax) ;
    static bool range2MinMax(const ODF &odf,const QString &range, double &vmin, double &vmax, bool &isInteger) ;
    static RawConverter converter(const ODF &odf, const QString &range);
};
}
}

#endif // ILWIS3RANGE_H
