#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "bilateralfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(BilateralFilter)

BilateralFilter::BilateralFilter()
{
}

BilateralFilter::BilateralFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool BilateralFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::bilateralFilter(cvRaster, cvOutputRaster, _pixneighborhood, _sigmaColor, _sigmaSpace);

    return true;
}

Ilwis::OperationImplementation *BilateralFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BilateralFilter(metaid,expr);
}

Ilwis::OperationImplementation::State BilateralFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters(itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itUINT8 | itFLOAT | itDOUBLE | itCONTINUOUSCOLOR | itPALETTECOLOR); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source image or floating-point, 1-channel or color" } );

        _pixneighborhood = _expression.input<double>(1);
        _sigmaColor = _expression.input<double>(2);
        _sigmaSpace = _expression.input<double>(3);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 BilateralFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/bilateralfilter"},"opencv");
    operation.setSyntax("bilateralfilter(inputraster, radius-pixelneigborhood, sigmacolor, sigmaspace)");
    operation.setDescription(TR("Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator, used in edge detection"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("radius pixel neighborhood"),TR("Diameter of each pixel neighborhood that is used during filtering. If it is non-positive, it is computed from sigmaSpace"));
    operation.addInParameter(2,itDOUBLE , TR("sigma color"),TR("sigma in the color space.Larger value means that farther colors within the pixel neighborhood will be mixed together, resulting in larger areas of semi-equal color"));
    operation.addInParameter(3,itDOUBLE , TR("sigma space"),TR("sigma in the pixel space.Larger value means that farther pixels will influence each other as long as their colors are close enough. When pixel neighborhood>0, it specifies the size regardless of sigmaSpace. Otherwise, d is proportional to sigmaSpace"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"domain=numericdomain");
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with reduced noise"));
    operation.setKeywords("image processing,raster,noise reduction, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
