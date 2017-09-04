#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "medianblurfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(MedianBlurFilter)

MedianBlurFilter::MedianBlurFilter()
{
}

MedianBlurFilter::MedianBlurFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool MedianBlurFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::medianBlur(cvRaster, cvOutputRaster,  _kernelSize);

    return true;
}

Ilwis::OperationImplementation *MedianBlurFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new MedianBlurFilter(metaid,expr);
}

Ilwis::OperationImplementation::State MedianBlurFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itNUMBER | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source: image or floating-point, 1-channel or color" } );

        _kernelSize = _expression.input<int>(1);

        OperationHelper::check([&] ()->bool { return _kernelSize > 0; },
        {ERR_ILLEGAL_VALUE_2,"kernel size", QString::number(_kernelSize) } );


        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 MedianBlurFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/medianblurfilter"},"opencv");
    operation.setSyntax("medianblurfilter(inputraster, kernelsize)");
    operation.setDescription(TR("smoothes an image using the median filter with the ksize  x ksize aperture"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("kernel size"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with smoothend features"));
    operation.setKeywords("image processing,raster,noise reduction, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
