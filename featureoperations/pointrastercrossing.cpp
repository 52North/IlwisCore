#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "raster.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operationhelperfeatures.h"
#include "operation.h"
#include "pointrastercrossing.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(PointRasterCrossing)

PointRasterCrossing::PointRasterCrossing()
{
}

PointRasterCrossing::PointRasterCrossing(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

bool PointRasterCrossing::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    AttributeTable table = _outputFeatures->attributeTable();
    quint32 record = 0;
    for(const auto& infeature : _inputFeatures){
        if ( infeature->geometryType() != itPOINT)
            continue;

        UPFeatureI& newFeature = _outputFeatures->newFeatureFrom(infeature);
        auto datarecord = infeature->recordRef();
        newFeature->record(datarecord);
        const geos::geom::Coordinate *crd = newFeature->geometry()->getCoordinate();
        if (!crd)
            continue;
        Coordinate coord =  _doCoordTransform ? _outputFeatures->coordinateSystem()->coord2coord(_inputRaster->coordinateSystem(), *crd) : *crd;

        Pixel pix = _inputRaster->georeference()->coord2Pixel(coord);
        for(int z = 0; z < _inputRaster->size().zsize(); ++z){
            pix.z = z;
            double v = _inputRaster->pix2value(pix);
            table->setCell(_startColumn + z,record,v);

        }
        ++record;
    }

    if ( ctx != 0) {
        QVariant value;
        value.setValue<IFeatureCoverage>(_outputFeatures);
        ctx->setOutput(symTable, value, _outputFeatures->name(), itFEATURE,_outputFeatures->source());
    }

    return true;

}

OperationImplementation *PointRasterCrossing::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new PointRasterCrossing(metaid, expr)   ;
}

quint64 PointRasterCrossing::createMetadata()
{
    OperationResource operation({"ilwis://operations/pointrastercrossing"});
    operation.setSyntax("pointrastercrossing(inputpointmap, inputgridcoverage)");
    operation.setDescription(TR("adds columns to the attribute table with for each column the values of intersection of the raster coverage bands with the pointmap"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itPOINT,  TR("intersecting point coverage"),TR("The point coverage with which the raster is to be crossed"));
    operation.addInParameter(1,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.addInParameter(2,itSTRING, TR("prefix"),TR("optional prefix to create column names, if not present the system will use what is available(domain,/mapnam)"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPOINT, TR("output point coverage"), TR("output point coverage with the extended attribute table"));
    operation.setKeywords("raster, point, intersection, cross");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State PointRasterCrossing::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    QString points = _expression.parm(0).value();
    QString raster = _expression.parm(1).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputFeatures.prepare(points, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,points,"");
        return sPREPAREFAILED;
    }

    if (!_inputRaster.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }

    if ( _inputFeatures->featureCount(itPOINT) == 0){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,TR("number of points"), TR("pointrastercrossing operation"));
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 3){
        _prefix = _expression.parm(2).value();
    }
    AttributeTable attributes = makeAttributeTable(_inputFeatures->attributeTable());

    _outputFeatures = OperationHelperFeatures::initialize(_inputFeatures,itPOINT,itCOORDSYSTEM | itDOMAIN | itENVELOPE);
    _outputFeatures->attributeTable(attributes);
    if (outputName != sUNDEF){
        _outputFeatures->name(outputName);
        _outputFeatures->attributeTable()->name(outputName);
    }

    _doCoordTransform = _inputRaster->coordinateSystem() != _outputFeatures->coordinateSystem();

    return sPREPARED;
}

QString PointRasterCrossing::columnName(const QVariant& trackIndexValue)
{
    QString columnName;
    if ( _prefix == "") {
    columnName = _inputRaster->datadef().domain()->ilwisType() == itNUMERICDOMAIN ?
                QString("%1_%2").arg(_inputRaster->name()).arg(trackIndexValue.toString()) :
                trackIndexValue.toString();
    } else {
        columnName = _prefix + "_" + trackIndexValue.toString();
    }
    return columnName;
}

AttributeTable PointRasterCrossing::makeAttributeTable(const ITable& inputTable){
    AttributeTable tbl;
    tbl.prepare();
    for(int col = 0; col < inputTable->columnCount(); ++col){
        tbl->addColumn(inputTable->columndefinition(col));
    }
    std::vector<QVariant> index = _inputRaster->indexValues();
    _startColumn = _inputFeatures->attributeTable()->columnCount();
    for(auto indexValue : index){

        QString colName = columnName(indexValue);
        if ( tbl->columnIndex(colName) == iUNDEF){
            tbl->addColumn(colName, _inputRaster->datadef().domain());
        }
    }
    return tbl;
}
