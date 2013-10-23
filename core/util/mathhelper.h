#ifndef MATHHELPER_H
#define MATHHELPER_H

namespace Ilwis {

struct Coefficient{
    Coefficient(double x=0, double y=0) : _x(x), _y(y){}
    double _x,_y;
};

class MathHelper
{
public:
    MathHelper();
    static bool findOblique(int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef, bool useCols);
    static bool findPolynom(int iTerms, int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef);
};
}
#endif // MATHHELPER_H
