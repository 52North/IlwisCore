#ifndef OPENCVOPERATION_H
#define OPENCVOPERATION_H

namespace Ilwis {
namespace OpenCV {


class OpenCVOperation : public OperationImplementation
{
public:
    OpenCVOperation();
    OpenCVOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    virtual bool doOperation(cv::Mat& inputRaster, cv::Mat& outputRaster) const{ return false; }

    bool execute(ExecutionContext *ctx, SymbolTable &symTable);
protected:
    IRasterCoverage _inputRaster;
    IRasterCoverage _outputRaster;
    bool _resolveUndefs = false;

    void createInputOutputRasters(quint64 copyProperties);
};
}
}

#endif // OPENCVOPERATION_H
