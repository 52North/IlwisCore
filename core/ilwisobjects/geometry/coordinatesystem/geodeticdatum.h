#ifndef DATUMDATA_H
#define DATUMDATA_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT GeodeticDatum : public Identity
{
public:
    enum DatumParameters{ dmDX, dmDY, dmDZ, dmRX, dmRY, dmRZ, dmSCALE,dmCENTERXR, dmCENTERYR, dmCENTERZR};
    GeodeticDatum(const QString &name=sUNDEF);

    QString area() const;
    void setArea(const QString& v);
    QString authority() const;
    void setAuthority(const QString &auth);
    double parameter(DatumParameters parm) const;
    bool isValid() const;
    void fromCode(const QString& code);
    void setWktName(const QString& name);
    QString toWKT(quint32 spaces=0) const;

    void set3TransformationParameters(double x, double z, double y);
    void set7TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale);
    void set10TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale, Coordinate center);
private:
    QString _area;
    QString _authority;
    std::vector<double> _datumParams;
    bool _isValid;
    QString _wkt;
};

}

#endif // DATUMDATA_H
