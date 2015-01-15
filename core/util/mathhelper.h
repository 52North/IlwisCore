#ifndef MATHHELPER_H
#define MATHHELPER_H

namespace Ilwis {

struct KERNELSHARED_EXPORT Coefficient{
    Coefficient(double x=0, double y=0) : _x(x), _y(y){}
    double _x,_y;
};

class KERNELSHARED_EXPORT MathHelper
{
public:
    MathHelper();
    static bool findOblique(int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef, bool useCols);
    static bool findPolynom(int iTerms, int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef);
    static NumericRange roundRange(double rmin, double rmax, double step);
};
}
#endif // MATHHELPER_H
