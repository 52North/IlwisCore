#ifndef DATUMDATA_H
#define DATUMDATA_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT GeodeticDatum : public Identity
{
public:
    enum DatumParameters{dmDX, dmDY, dmDZ, dmRX, dmRY, dmRZ, dmSCALE, dmCENTERXR, dmCENTERYR, dmCENTERZR};
    enum DatumTransformation{dtMolodensky, dtBursaWolf, dtBadekas};
    GeodeticDatum(const QString &name=sUNDEF);
    GeodeticDatum(std::vector<double>& datumParameters, const IEllipsoid &ellips);

    QString area() const;
    void setArea(const QString& v);
    QString authority() const;
    void setAuthority(const QString &auth);
    double parameter(DatumParameters parm) const;
    DatumTransformation getTransformationMode() const;
    bool isValid() const;
    void fromCode(const QString& code);
    void setWktName(const QString& name);
    QString toWKT(quint32 spaces=0) const;

    void set3TransformationParameters(const double x, const double y, const double z, const IEllipsoid &ellips);
    void set7TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale);
    void set10TransformationParameters(const double x, const double y, const double z, const double rx, const double ry, const double rz, const double scale, const Coordinate & center);

    GeodeticDatum *clone() const;
private:
    void getFromInternal(const QString & ellips);
    QString _area;
    QString _authority;
    std::vector<double> _datumParams;
    DatumTransformation _mode;
    bool _isValid;
    QString _wkt;
};

}

#endif // DATUMDATA_H
