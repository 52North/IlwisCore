#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "histogramequalization.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(HistogramEqualization)

HistogramEqualization::HistogramEqualization()
{
}

HistogramEqualization::HistogramEqualization(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool HistogramEqualization::doOperation(cv::Mat &cvRaster, cv::Mat &cvOutputRaster) const
{
    cv::equalizeHist(cvRaster, cvOutputRaster);

    return true;
}

Ilwis::OperationImplementation *HistogramEqualization::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new HistogramEqualization(metaid,expr);
}

Ilwis::OperationImplementation::State HistogramEqualization::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        createInputOutputRasters( itCOORDSYSTEM | itGEOREF | itENVELOPE | itDOMAIN);

        OperationHelper::check([&] ()->bool { return hasType(_inputRaster->datadef().domain()->valueType(), itUINT8); },
            {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "wrong data type; Allowed is Source: domain image" } );

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 HistogramEqualization::createMetadata()
{
    OperationResource operation({"ilwis://operations/histogramhqualization"},"opencv");
    operation.setLongName("Histogram Equalization");
    operation.setSyntax("HistogramEqualization(inputraster1)");
    operation.setDescription(TR("Equalizes the histogram of a grayscale image,normalizes the brightness and increases the contrast of the image"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itRASTER , TR("first rastercoverage"), TR("Source domain image single channel image"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output raster"));
    operation.setKeywords("image processing,raster, contrast");

    mastercatalog()->addItems({operation});
    return operation.id();
}
