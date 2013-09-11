#ifndef INDEXSLICER_H
#define INDEXSLICER_H

namespace Ilwis {
class KERNELSHARED_EXPORT IndexSlicer
{
public:
    IndexSlicer(const IGridCoverage& cov);
    void grid(const IGridCoverage& cov);
    IGridCoverage operator()(const QString& item1, const QString& item2=sUNDEF, const QString& step=sUNDEF);
private:
    QString makeBaseName() const;
    double findIndexNumericalItem(const Ilwis::IDomain &indexDomain, double itemIndex) const;
    double findIndexNumber(const IDomain &indexDomain, double itemIndex) const;
    QString makeExpression(double index, const QString &basename, QString &cname);

    IGridCoverage _raster;
};
}

#endif // INDEXSLICER_H
