#ifndef HistogramEqualization_H
#define HistogramEqualization_H

namespace Ilwis {
namespace OpenCV {


class HistogramEqualization : public OpenCVOperation
{
public:
    HistogramEqualization();


    HistogramEqualization(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(HistogramEqualization);

private:

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;


};
}
}

#endif // HistogramEqualization_H
