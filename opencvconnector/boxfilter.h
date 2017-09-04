#ifndef BoxFilter_H
#define BoxFilter_H

namespace Ilwis {
namespace OpenCV {


class BoxFilter : public OpenCVOperation
{
public:
    BoxFilter();


    BoxFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(BoxFilter);

private:

    cv::Size _kernelSize;
    bool _normalize = true;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // BoxFilter_H
