#ifndef MedianBlurFilter_H
#define MedianBlurFilter_H

namespace Ilwis {
namespace OpenCV {


class MedianBlurFilter : public OpenCVOperation
{
public:
    MedianBlurFilter();


    MedianBlurFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(MedianBlurFilter);

private:

    int _kernelSize;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // MedianBlurFilter_H
