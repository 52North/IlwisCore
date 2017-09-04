#ifndef ErodeFilter_H
#define ErodeFilter_H

namespace Ilwis {
namespace OpenCV {


class ErodeFilter : public OpenCVOperation
{
public:
    ErodeFilter();


    ErodeFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(ErodeFilter);

private:

    cv::Size _kernelSize;
    cv::Point _anchor;
    quint32 _iterations;
    int _shape;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // ErodeFilter_H
