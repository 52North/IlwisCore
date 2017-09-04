#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "boxfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(BoxFilter)

BoxFilter::BoxFilter()
{
}

BoxFilter::BoxFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool BoxFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::boxFilter(cvRaster, cvOutputRaster, -1, _kernelSize, cv::Point(-1,-1), _normalize);

    return true;
}

Ilwis::OperationImplementation *BoxFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BoxFilter(metaid,expr);
}

Ilwis::OperationImplementation::State BoxFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itUINT8 | itFLOAT | itDOUBLE | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source: image or floating-point, 1-channel or color" } );

        int ksizex = _expression.input<int>(1);
        int ksizey = _expression.input<int>(2);

        _kernelSize = cv::Size(ksizex, ksizey);
        if ( _expression.parameterCount() == 4)
            _normalize = _expression.input<bool>(3);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 BoxFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/boxfilter"},"opencv");
    operation.setSyntax("boxfilter(inputraster, kernelsizeX, kernelsizeY[,normalized])");
    operation.setDescription(TR("Blurs an image using the box filter"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("kernel width"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addInParameter(2,itINTEGER , TR("kernel height"),TR("This is the neighborhood where the local variance will be calculated, and where pixels will contribute (in a weighted manner)"));
    operation.addOptionalInParameter(3,itBOOL , TR("normalized"),TR("specifying whether the kernel is normalized by its area or not.Unnormalized box filter for covariance matrices of image derivatives"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with blurred features and reduced noise"));
    operation.setKeywords("image processing,raster,noise reduction, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
