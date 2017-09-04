#ifndef BILATERALfILTER_H
#define BILATERALfILTER_H

namespace Ilwis {
namespace OpenCV {


class BilateralFilter : public OpenCVOperation
{
public:
    BilateralFilter();


    BilateralFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(BilateralFilter);

private:
    qint32 _pixneighborhood;
    double _sigmaColor;
    double _sigmaSpace;


};
}
}

#endif // BILATERALfILTER_H
