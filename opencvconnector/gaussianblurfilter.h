#ifndef GaussianBlurFilter_H
#define GaussianBlurFilter_H

namespace Ilwis {
namespace OpenCV {


class GaussianBlurFilter : public OpenCVOperation
{
public:
    GaussianBlurFilter();


    GaussianBlurFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(GaussianBlurFilter);

private:

    cv::Size _kernelSize;
    double _sigmaX;
    double _sigmaY;


    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // GaussianBlurFilter_H
