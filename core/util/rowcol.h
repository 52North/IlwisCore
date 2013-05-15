#ifndef ROWCOL_H
#define ROWCOL_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT RowCol
{
public:
    RowCol();

    RowCol(double r, double c);
    RowCol(QString& pair);
    double row(bool subpixel=false) const;
    void setRow(double r);
    double col(bool subpixel=false) const;
    void setCol(double c);
    bool operator==(const RowCol& rc) const;
    bool operator!=(const RowCol& rc) const;
    bool isValid() const;
    QString toString() const;

private:
    double _row, _col;


};
}

#define rcUNDEF Ilwis::RowCol(iUNDEF, iUNDEF)

#endif // ROWCOL_H
