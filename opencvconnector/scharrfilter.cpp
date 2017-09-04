#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "scharrfilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(ScharrFilter)

ScharrFilter::ScharrFilter()
{
}

ScharrFilter::ScharrFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool ScharrFilter::doOperation(cv::Mat& cvRaster, cv::Mat& cvOutputRaster) const
{
    cv::GaussianBlur(cvRaster,cvRaster,cv::Size(3,3),0, 0, cv::BORDER_DEFAULT ); // reduce noise

    cv::Mat cvGrey;

    if ( _inputRaster->datadef().domain()->ilwisType() == itCOLORDOMAIN)
        cv::cvtColor( cvRaster, cvGrey, CV_RGB2GRAY );
    else
        cvGrey = cvRaster;

    cv::Mat gradient;
    cv::Scharr( cvGrey, gradient, _destdepth, _xorder, _yorder);

    cv::convertScaleAbs(gradient, cvOutputRaster);

    return true;
}

Ilwis::OperationImplementation *ScharrFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ScharrFilter(metaid,expr);
}

Ilwis::OperationImplementation::State ScharrFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters(itCOORDSYSTEM | itGEOREF | itENVELOPE | itRASTERSIZE);

        IDomain dom;
        if ( _inputRaster->datadef().domain()->valueType() == itUINT8)
            dom.prepare("image16");
        else {
            dom.prepare("value");
        }
        _outputRaster->datadefRef() = DataDefinition(dom);

        int xorder = _expression.input<int>(1);
        OperationHelper::check([&] ()->bool {return (xorder >=0 || xorder <= 2);},
            {ERR_ILLEGAL_VALUE_2,"Scharr filter, x-order", QString::number(xorder)} );

        int yorder = _expression.input<int>(2);
        OperationHelper::check([&] ()->bool {return (yorder >=0 || yorder <= 2);},
            {ERR_ILLEGAL_VALUE_2,"Scharr filter, y-order", QString::number(yorder)} );

        _xorder = xorder;
        _yorder = yorder;

        _sourcedepth =  hasType(_inputRaster->datadef().domain()->valueType(), (itCOLOR | itPALETTECOLOR)) ? 3 : 1;

        OperationHelper::check([&] ()->bool {return OpenCVHelper::determineCVType(_inputRaster->datadef().domain()->valueType(), _sourcedepth);},
            {ERR_OPERATION_NOTSUPPORTED2, "Value type", "Scharr filter"});

        _destdepth = _sourcedepth == CV_8U ? CV_16U : -1;

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 ScharrFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/scharrfilter"},"opencv");
    operation.setSyntax("scharrfilter(inputraster, x-order-derivative, y-order-derivative)");
    operation.setDescription(TR("Calculates the first, second, third, or mixed image derivatives using an extended scharr operator, it is more accurate than the Sobel operator"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"),TR("input rastercoverage, color or image(grey) domain"));
    operation.addInParameter(1,itPOSITIVEINTEGER , TR("X order derivative"),TR("order of the derivative in x direction"));
    operation.addInParameter(2,itPOSITIVEINTEGER , TR("Y order derivative"),TR("order of the derivative in y direction"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output raster with edges highlighted"));
    operation.setKeywords("image processing,raster,edge detection,filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
