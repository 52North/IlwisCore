#ifndef ScharrDERIVATIVES_H
#define ScharrDERIVATIVES_H

namespace Ilwis {
namespace OpenCV {


class ScharrFilter : public OpenCVOperation
{
public:
    ScharrFilter();


    ScharrFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(ScharrFilter);

private:
    quint8 _xorder;
    quint8 _yorder;
    qint8 _sourcedepth;
    qint8 _destdepth;

    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
};
}
}

#endif // ScharrDERIVATIVES_H
