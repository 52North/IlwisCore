#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "laplacefilter.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(LaplaceFilter)

LaplaceFilter::LaplaceFilter()
{
}

LaplaceFilter::LaplaceFilter(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool LaplaceFilter::doOperation(cv::Mat& cvRaster, cv::Mat& cvOutputRaster) const
{
    cv::GaussianBlur(cvRaster,cvRaster,cv::Size(3,3),0, 0, cv::BORDER_DEFAULT ); // reduce noise

    cv::Mat cvGrey;

    if ( _inputRaster->datadef().domain()->ilwisType() == itCOLORDOMAIN)
        cv::cvtColor( cvRaster, cvGrey, CV_RGB2GRAY );
    else
        cvGrey = cvRaster;

    cv::Laplacian( cvGrey, cvOutputRaster, _sourcedepth, _kernelSize);

    cv::convertScaleAbs(cvOutputRaster, cvOutputRaster);

    return true;

}

Ilwis::OperationImplementation *LaplaceFilter::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new LaplaceFilter(metaid,expr);
}

Ilwis::OperationImplementation::State LaplaceFilter::prepare(ExecutionContext *ctx, const SymbolTable &st)
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


        _sourcedepth =  hasType(_inputRaster->datadef().domain()->valueType(), (itCOLOR | itPALETTECOLOR)) ? 3 : 1;

        OperationHelper::check([&] ()->bool {return OpenCVHelper::determineCVType(_inputRaster->datadef().domain()->valueType(), _sourcedepth);},
            {ERR_OPERATION_NOTSUPPORTED2, "Value type", "Laplace filter"});


        _kernelSize = _expression.parameterCount() == 2 ? _expression.input<quint32>(1) : 1;
        std::vector<int> possibleValue = {1,3,5,7};
        OperationHelper::check([&] ()->bool { return (std::find(possibleValue.begin(), possibleValue.end(), _kernelSize) != possibleValue.end());},
            {ERR_ILLEGAL_VALUE_2, "Laplace filter, kernel size", QString::number(_kernelSize)});

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 LaplaceFilter::createMetadata()
{
    OperationResource operation({"ilwis://operations/laplacefilter"},"opencv");
    operation.setSyntax("laplacefilter(inputraster ,1|!3|5|7)");
    operation.setDescription(TR("highlights regions of rapid intensity change, used in edge detection"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER , TR("rastercoverage"),TR("input rastercoverage, color or image(grey) domain"));
    operation.addInParameter(1,itPOSITIVEINTEGER , TR("Size of the kernel"),TR("size of the extended Sobel kernel; it must be 1, 3, 5, or 7"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"),TR("output raster with edges highlighted"));
    operation.setKeywords("image processing,raster,edge detection, filter");

    mastercatalog()->addItems({operation});
    return operation.id();
}
