#ifndef SOBELDERIVATIVES_H
#define SOBELDERIVATIVES_H

namespace Ilwis {
namespace OpenCV {


class SobelFilter : public OpenCVOperation
{
public:
    SobelFilter();


    SobelFilter(quint64 metaid, const Ilwis::OperationExpression &expr);

    //bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SobelFilter);

private:
    quint8 _xorder;
    quint8 _yorder;
    quint8 _kernelSize;
    qint8 _sourcedepth;
    qint8 _destdepth;


    bool doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const;
};
}
}

#endif // SOBELDERIVATIVES_H
