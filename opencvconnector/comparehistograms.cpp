#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "comparehistograms.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(CompareHistograms)

CompareHistograms::CompareHistograms()
{
}

CompareHistograms::CompareHistograms(quint64 metaid, const Ilwis::OperationExpression &expr) : OpenCVOperation(metaid,expr  )
{
}

bool CompareHistograms::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    try{
        std::vector<double> _markers;
        cv::SparseMat histogram1, histogram2;
        auto indexes1 = _inputRaster1->stackDefinition().indexes();
        auto indexes2 = _inputRaster2->stackDefinition().indexes();
        for(int i =0; i < indexes1.size(); ++i ){
            PixelIterator inputIter1 = _inputRaster1->band(indexes1[i]);
            PixelIterator inputIter2 = _inputRaster2->band(indexes2[i]);

            OpenCVHelper::createHistogram(inputIter1,histogram1, _accumulate);
            OpenCVHelper::createHistogram(inputIter2,histogram2, _accumulate);

            if (!_accumulate)
                _markers.push_back(cv::compareHist(histogram1, histogram2, _marker));

        }
        if ( _accumulate)
            _markers.push_back(cv::compareHist(histogram1, histogram2, _marker));

        if ( ctx != 0) {
            QVariant value;
            value.setValue<std::vector<double>>(_markers);
            ctx->addOutput(symTable, value, sUNDEF, itDOUBLE | itCOLLECTION, Resource());
            return true;
        }


    }catch(cv::Exception& ex){
        ERROR0(QString::fromStdString(ex.msg));
    }
    return false;
}

Ilwis::OperationImplementation *CompareHistograms::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CompareHistograms(metaid,expr);
}

Ilwis::OperationImplementation::State CompareHistograms::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster1.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        OperationHelper::check([&] ()->bool { return _inputRaster2.prepare(_expression.input<QString>(1), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );

        OperationHelper::check([&] ()->bool { return _inputRaster2->size().zsize() == _inputRaster2->size().zsize(); },
        {ERR_ILLEGAL_VALUE_2, TR("band numbers"),  QString("%1 != %2").arg(_inputRaster2->size().zsize()).arg(_inputRaster1->size().zsize())} );

        QString method = _expression.input<QString>(2);
        std::map<QString, int> options = { {"correlation",CV_COMP_CORREL},{"chi-square",CV_COMP_CHISQR},{"intersection",CV_COMP_INTERSECT },{"bhattacharyya-distance",CV_COMP_BHATTACHARYYA },{"hellinger-distance",CV_COMP_HELLINGER}};
        auto iter = options.find(method);
        OperationHelper::check([&] ()->bool { return iter != options.end(); },
        {ERR_ILLEGAL_VALUE_2,TR("parameter in comparehistograms"), method } );

        _marker = iter->second;

        if ( _expression.parameterCount() == 4)
            _accumulate = _expression.input<bool>(3);

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;
}

quint64 CompareHistograms::createMetadata()
{
    OperationResource operation({"ilwis://operations/comparehistograms"},"opencv");
    operation.setSyntax("CompareHistograms(inputraster1, inputraster2,statisticalmarker=correlation | chi-square | intersection | bhattacharyya-distance | hellinger-distance[,accumulated)");
    operation.setDescription(TR("Blurs an image using the box filter"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itRASTER , TR("first rastercoverage"),TR("raster coverage with a numerical or color domain"));
    operation.addInParameter(1,itRASTER , TR("second rastercoverage"),TR("raster coverage with the same domain as  the first coverage") );
    operation.addInParameter(2,itSTRING , TR("marker"),TR("statistical marker number to be calculated. It can be correlation, chi-square, intersection, bhattacharyya distance or hellinger distance"));
    operation.addInParameter(3,itBOOL , TR("acumulate"),TR("if there is more than one band it treats the whole raster as one; it is optional and the default is false") );
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itDOUBLE | itCOLLECTION, TR("statistical Marker"),TR("a raster with blurred features and reduced noise"));
    operation.setKeywords("image processing,raster,statistics");

    mastercatalog()->addItems({operation});
    return operation.id();
}
