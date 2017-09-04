#ifndef AdaptiveBilateralFilter_H
#define AdaptiveBilateralFilter_H

namespace Ilwis {
namespace OpenCV {


class AdaptiveBilateralFilter : public OpenCVOperation
{
public:
    AdaptiveBilateralFilter();


    AdaptiveBilateralFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(AdaptiveBilateralFilter);

private:
    double _sigmaSpace;
    double _maxSigmaColor;
    cv::Size _kernelSize;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // AdaptiveBilateralFilter_H
