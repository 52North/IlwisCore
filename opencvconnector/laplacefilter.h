#ifndef LAPLACEFILTER_H
#define LAPLACEFILTER_H

namespace Ilwis {
namespace OpenCV {


class LaplaceFilter : public OpenCVOperation
{
public:
    LaplaceFilter();


    LaplaceFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(LaplaceFilter);

private:
    quint8 _kernelSize;
    qint8 _sourcedepth;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
};
}
}

#endif // SOBELDERIVATIVES_H
