#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "gaussianblurfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(GaussianBlurFilter)

GaussianBlurFilter::GaussianBlurFilter()
{
}

GaussianBlurFilter::GaussianBlurFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool GaussianBlurFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::GaussianBlur(cvRaster, cvOutputRaster, _kernelSize, _sigmaX, _sigmaY);

    return true;
}

Ilwis::OperationImplementation *GaussianBlurFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new GaussianBlurFilter(metaid,expr);
}

Ilwis::OperationImplementation::State GaussianBlurFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itUINT8 | itFLOAT | itDOUBLE | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source: image or floating-point, 1-channel or color" } );

        int ksizex = _expression.input<int>(1);
        int ksizey = _expression.input<int>(2);

        OperationHelper::check([&] ()->bool { return ksizex > 0 && ksizey > 0; },
        {ERR_ILLEGAL_VALUE_2,"kernel size", QString::number(ksizex) + "x" + QString::number(ksizey) } );

        _kernelSize = cv::Size(ksizex, ksizey);
        _sigmaX = _expression.input<double>(3);
        OperationHelper::check([&] ()->bool { return _sigmaX >= 0; },
         {ERR_ILLEGAL_VALUE_2,"GaussianBlur,SigmaX", QString::number(_sigmaX)} );

        if ( _expression.parameterCount() == 5)
            _sigmaY = _expression.input<double>(4);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 GaussianBlurFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/gaussianblurfilter"},"opencv");
    operation.setSyntax("gaussianblurfilter(inputraster, kernelsizeX, kernelsizeY, sigmaX[,sigmaY])");
    operation.setDescription(TR(" blurring an image by a Gaussian function to reduce image noise and reduce detail"));
    operation.setInParameterCount({4,5});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("kernel width"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addInParameter(2,itINTEGER , TR("kernel height"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addInParameter(3,itDOUBLE , TR("sigmax"),TR("gaussian kernel standard deviation in X direction.If both sigmas are zeros, they are computed from the kernel size"));
    operation.addOptionalInParameter(4,itDOUBLE , TR("sigmay"),TR("gaussian kernel standard deviation in Y direction. If set to 0, the SigmaX value is taken optional"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with blurred features and reduced noise"));
    operation.setKeywords("image processing,raster,noise reduction, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
