#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "erodefilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(ErodeFilter)

ErodeFilter::ErodeFilter()
{
}

ErodeFilter::ErodeFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool ErodeFilter::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::Mat cvKernel = cv::getStructuringElement(_shape,_kernelSize,_anchor);

    cv::dilate(cvRaster, cvOutputRaster, cvKernel, cv::Point(-1,-1),_iterations);

    return true;
}

Ilwis::OperationImplementation *ErodeFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ErodeFilter(metaid,expr);
}

Ilwis::OperationImplementation::State ErodeFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);


        int iterations = _expression.input<int>(1);
        OperationHelper::check([&] ()->bool { return iterations > 0; },
        {ERR_ILLEGAL_VALUE_2,"iteration number", QString::number(iterations) } );
        _iterations = iterations;
        QString shape = _expression.input<QString>(2).toLower();
        if ( shape == "rectangle")
            _shape = cv::MORPH_RECT;
        else if ( shape == "ellipse")
            _shape = cv::MORPH_ELLIPSE;
        else if ( shape == "cross")
            _shape = cv::MORPH_CROSS;
        else {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("illegal kernel shape"), shape);
            return sPREPAREFAILED;
        }

        int ksizex = _expression.input<int>(3);
        int ksizey = _expression.input<int>(4);

        OperationHelper::check([&] ()->bool { return ksizex > 0 && ksizey > 0; },
        {ERR_ILLEGAL_VALUE_2,"kernel size", QString::number(ksizex) + "x" + QString::number(ksizey) } );

        _kernelSize = cv::Size(ksizex, ksizey);
        if ( _expression.parameterCount() == 7){
            int x = _expression.input<int>(5);
            int y = _expression.input<int>(6);
            _anchor = cv::Point(x,y);
        }else
            _anchor = cv::Point(-1,-1);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 ErodeFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/erodefilter"},"opencv");
    operation.setSyntax("erodefilter(inputraster,iterations, kerneltype=!rectangle|ellipse|cross,kernel-x-size, kernel-y-size[,x-anchor, y-anchor])");
    operation.setDescription(TR("Erodes an image by using a specific structuring element."));
    operation.setInParameterCount({5,7});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"));
    operation.addInParameter(1,itINTEGER , TR("iterations"),TR("number of times erode is applied"));
    operation.addInParameter(2,itSTRING , TR("kernel-shape"),TR("structuring element for the filter which can either be a rectangle, ellipse or cross"));
    operation.addInParameter(3,itPOSITIVEINTEGER , TR("kernel width"),TR("width of the structuring element used in the flter"));
    operation.addInParameter(4,itPOSITIVEINTEGER , TR("kernel height"),TR("height of the structuring element used in the flter"));
    operation.addOptionalInParameter(5,itINTEGER , TR("anchors x-position"),TR("anchors position wihtin the kernel. the default is center"));
    operation.addOptionalInParameter(6,itINTEGER , TR("anchors y-position"),TR("anchors position wihtin the kernel. the default is center"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("a raster with morphology changed of affected structures in the raster"));
    operation.setKeywords("image processing,raster,morphology, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
