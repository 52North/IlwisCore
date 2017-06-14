#include <QString>
#include <functional>
#include <future>

#include "geos/geom/Geometry.h"
#include "geos/operation/buffer/BufferOp.h"
#include "geos/operation/buffer/BufferParameters.h"
#include "geos/util/GEOSException.h"
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "geometryhelper.h"
#include "featureiterator.h"
#include "buffer.h"


using namespace Ilwis;
using namespace FeatureOperations;

namespace GeosBuffer = ::geos::operation::buffer;


REGISTER_OPERATION(Buffer)



Buffer::Buffer()
{
}


Buffer::Buffer(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Buffer::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    try{
        FeatureIterator inFeatureIter(_inputFeatures);
        FeatureIterator end = inFeatureIter.end();

        quint64 currentCount = 0;

        while(inFeatureIter != end) {
            // create new feature and buffer
            SPFeatureI newOutputFeature = _outputFeatures->newFeatureFrom(*inFeatureIter);

            // CAP_ROUND seems to be the only end cap style working in the geos lib, so
            // we use it explicitly
            ::geos::geom::Geometry *geomBuffer = newOutputFeature->geometry()->buffer(_distance, GeosBuffer::BufferParameters::DEFAULT_QUADRANT_SEGMENTS, GeosBuffer::BufferParameters::CAP_ROUND);

            newOutputFeature->geometry(geomBuffer);
            auto infeature = *inFeatureIter;

            copyAttributeValues(newOutputFeature, infeature);

            ++inFeatureIter;

            updateTranquilizer(currentCount++, 1000);
        }
    } catch(geos::util::GEOSException& exc){
        ERROR0(QString(exc.what()));
        return false;
    }


    QVariant value;
    value.setValue<IFeatureCoverage>(_outputFeatures);
    logOperation(_outputFeatures, _expression);
    ctx->setOutput(symTable, value, _outputFeatures->name(), itFEATURE, _outputFeatures->resource());


    return true;
}

OperationImplementation *Buffer::create(quint64 metaid, const OperationExpression &expr)
{
    return new Buffer(metaid, expr);
}

OperationImplementation::State Buffer::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString inputFeatureCoverage = _expression.parm(0).value();
    QString inputBufferDistance = _expression.parm(1).value();
    QString inputQuadSegments = _expression.parm(2).value();
    QString inputEndCapStyle = _expression.parm(3).value();

    // retrieve from output params
    QString outputName = _expression.parm(0,false).value();

    if (!_inputFeatures.prepare(inputFeatureCoverage, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputFeatureCoverage,"");
        return sPREPAREFAILED;
    }

    bool ok;

    _distance = inputBufferDistance.toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","2");
        return sPREPAREFAILED;
    }

    _nQuadrantSegments = inputQuadSegments.toInt(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","3");
        return sPREPAREFAILED;
    }

    _endCapStyle = GeosBuffer::BufferParameters::CAP_ROUND;

    _nQuadrantSegments = inputQuadSegments.toInt(&ok);
    if (inputEndCapStyle == "round")
        _endCapStyle = GeosBuffer::BufferParameters::CAP_ROUND;
    else if(inputEndCapStyle == "butt")
        _endCapStyle = GeosBuffer::BufferParameters::CAP_FLAT;
    else if(inputEndCapStyle == "square")
        _endCapStyle = GeosBuffer::BufferParameters::CAP_SQUARE;
    else {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","4");
        return sPREPAREFAILED;
    }

    _outputFeatures =  OperationHelperFeatures::initialize(_inputFeatures,itFEATURE, itCOORDSYSTEM | itENVELOPE);

    addAttributeColumns();

    if (outputName != sUNDEF){
        _outputFeatures->name(outputName);
        //_outputFeatures->attributeTable()->name(outputName);
    }

    initialize(_inputFeatures->featureCount() );

    return sPREPARED;
}



quint64 Buffer::createMetadata()
{
    OperationResource operation({"ilwis://operations/buffer"});
    operation.setSyntax("buffer(coverage,distance, nQuadrantSegments, endCapStyle=!round|butt|square)");
    operation.setDescription(TR("returns a buffer around the given geometry, with a radius upto the given distance"));
    operation.setInParameterCount({4});
    operation.addInParameter(0, itFEATURE,TR("input coverage"), TR("input coverage based on which the return buffer"));
    operation.addInParameter(1, itDOUBLE, TR("buffer distance"), TR("buffer distance around the polygon"));
    operation.addInParameter(2, itINT32, TR("quadrant segments"), TR("number of segments used to approximate a quarter circle"));
    operation.addInParameter(3, itSTRING, TR("end cap style"), TR("buffer end cap style"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output buffer coverage"),TR("coverage containing the buffer around the input coverage"));
    operation.setKeywords("features, buffer, vector");

    mastercatalog()->addItems({operation});
    return operation.id();
}


void Buffer::addAttributeColumns(){
    quint32 defCount = _inputFeatures->attributeDefinitions().definitionCount();

    for(int idx = 0; idx < defCount; idx++){
        QString colName = _inputFeatures->attributeDefinitions().columndefinition(idx).name();
        IDomain colDomain =_inputFeatures->attributeDefinitions().columndefinition(idx).datadef().domain();

        _outputFeatures->attributeDefinitionsRef().addColumn(ColumnDefinition(colName, colDomain));
    }
}

void Buffer::copyAttributeValues(SPFeatureI &outputFeature, SPFeatureI &inputFeature){
    for(int col = 0; col < inputFeature->attributeColumnCount(); ++col){
        // copy attribute value from inputFeature
        QVariant cellValue = inputFeature->cell(col);
        outputFeature->setCell(col, cellValue);
    }
}

