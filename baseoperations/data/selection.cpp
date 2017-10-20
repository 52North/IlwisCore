#include "raster.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "table.h"
#include "domain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "selectionbase.h"
#include "selection.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SelectionRaster)

SelectionRaster::SelectionRaster()
{
}


SelectionRaster::SelectionRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : SelectionBase(metaid, expr)
{
}

SelectionRaster::~SelectionRaster()
{
}


bool SelectionRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();

    std::map<Raw, quint32> raw2record;
    int keyColumn = _inputAttributeTable.isValid() ? _inputAttributeTable->columnIndex(inputRaster->primaryKey()) : iUNDEF;
    if (keyColumn != iUNDEF){
        std::vector<QVariant> values = _inputAttributeTable->column(keyColumn);
        for(quint32 rec=0; rec < values.size(); ++rec){
            Raw r = values[rec].toDouble();
            if ( !isNumericalUndef(r)){
                raw2record[r] = rec;
            }
        }
    }

    std::vector<int> extraAtrrib = organizeAttributes();


    std::vector<QString> selectionBands = bands(inputRaster);
    initialize(outputRaster->size().linearSize() * selectionBands.size());

    PixelIterator iterOut(outputRaster);
    int count = 0;
    bool numeric = outputRaster->datadef().domain()->ilwisType() == itNUMERICDOMAIN;
    for(QString band : selectionBands){
        PixelIterator iterIn = inputRaster->band(band, _box);

        PixelIterator iterEnd = iterIn.end();
        while(iterIn != iterEnd) {
            bool ok = true;
            double pixValue = *iterIn;
            double matchValue = pixValue;

            for(const auto& epart : _expressionparts){
                bool partOk = epart.match(iterIn.position(), matchValue,this);
                if ( epart._andor != loNONE)
                    ok =  epart._andor == loAND ? ok && partOk : ok || partOk;
                else
                    ok &= partOk;
                if (epart._type == ExpressionPart::ptATTRIBUTE && extraAtrrib.size() == 1){
                    if ( pixValue < 0 || pixValue >= _inputAttributeTable->recordCount()){
                        ok = false;
                        continue;
                    }
                    // pixValue == ID; ID zero means undef, so subtract 1 to get correct record index.
                    if (pixValue > 0) {
                        if (keyColumn != iUNDEF){
                            auto iter = raw2record.find(pixValue);
                            if ( iter != raw2record.end()){
                                quint32 rec = iter->second;
                                const Record& record = _inputAttributeTable->record(rec);
                                pixValue = record.cell(extraAtrrib[0]).toDouble();
                            }else
                                pixValue = rUNDEF;
                        }

                    }
                    else
                        pixValue = rUNDEF;
                }
            }
            if ( ok){
                *iterOut = pixValue;
            }else
                *iterOut = rUNDEF;

            ++iterIn;
            ++iterOut;
            updateTranquilizer(++count, 100);
        }
        // if there is an attribute table we must copy the correct attributes and records
        if ( keyColumn != iUNDEF && _attTable.isValid()){
            for(int recIndex=0; recIndex < _inputAttributeTable->recordCount(); ++recIndex){
                const Record& rec = _inputAttributeTable->record(recIndex);
                for(int i=0; i < extraAtrrib.size(); ++i){
                    _attTable->setCell(i, recIndex, rec.cell(extraAtrrib[i]));
                }
            }
        }
    }
    if ( numeric)
        outputRaster->statistics(ContainerStatistics<double>::pBASIC);

    outputRaster->setAttributes(_attTable);
    QVariant value;
    value.setValue<IRasterCoverage>(outputRaster);
    logOperation(outputRaster, _expression);
    ctx->setOutput(symTable, value, outputRaster->name(), itRASTER,outputRaster->resource());
    return true;


}

Ilwis::OperationImplementation *SelectionRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SelectionRaster(metaid, expr);
}


Ilwis::OperationImplementation::State SelectionRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() != 2) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }
    IlwisTypes inputType = itRASTER;
    QString raster = _expression.parm(0).value();
    if (!_inputObj.prepare(raster, inputType)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    IRasterCoverage inputRaster = _inputObj.as<RasterCoverage>();
    _inputAttributeTable = inputRaster->attributeTable();
    quint64 copylist = itCOORDSYSTEM ;


    QString selector = _expression.parm(1).value();
    parseSelector(selector, inputRaster);

    std::vector<QString> selectionBands = bands(inputRaster);
     _box = boundingBox(_inputObj.as<RasterCoverage>());
    bool useOldGrf ;
    if ( _box.isNull() || !_box.isValid()){
        _box = inputRaster->size();
        useOldGrf = true;
    } else
        useOldGrf = false;

    if ( useOldGrf){
        copylist |= itGEOREF | itRASTERSIZE | itENVELOPE;
    }


    int selectedAttributes = attributeNames().size();
    if (selectedAttributes != 1)
        copylist |= itDOMAIN;

     _outputObj = OperationHelperRaster::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();

     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->name(outputName);

     if ( selectedAttributes > 1) {
         QString url = INTERNAL_CATALOG + "/" + outputName;
         Resource resource(url, itFLATTABLE);
         _attTable.prepare(resource);
     }
     if ( selectedAttributes == 1 && _inputAttributeTable.isValid()){
        QStringList names = attributeNames();
        //outputRaster->datadefRef().domain(_inputAttributeTable->columndefinition(names[0]).datadef().domain());
        outputRaster->setDataDefintions(_inputAttributeTable->columndefinition(names[0]).datadef().domain(), selectionBands, inputRaster->stackDefinition().domain());
     }else
         outputRaster->setDataDefintions(inputRaster->datadef().domain(), selectionBands, inputRaster->stackDefinition().domain());



     if ( (copylist & itGEOREF) == 0) {
         Resource resource(QUrl(INTERNAL_CATALOG + "/" + outputRaster->name() + "_grf_" + QString::number(outputRaster->id())),itGEOREF);
         resource.addProperty("size", IVARIANT(_box.size()));
         auto envelope = inputRaster->georeference()->pixel2Coord(_box);
         resource.addProperty("envelope", IVARIANT(envelope));
         resource.addProperty("coordinatesystem", IVARIANT(inputRaster->coordinateSystem()));
         resource.addProperty("name", _outputObj->name());
         resource.addProperty("centerofpixel",inputRaster->georeference()->centerOfPixel());
         IGeoReference  grf;
         grf.prepare(resource);
         outputRaster->georeference(grf);
         outputRaster->envelope(envelope);
    }


    return sPREPARED;
}

quint64 SelectionRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/selection"});
    operation.setLongName("Raster Selection");
    operation.setSyntax("selection(coverage,selection-definition)");
    operation.setDescription(TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING,  TR("selection-definition"),TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    operation.parameterNeedsQuotes(1);
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("selection"));
    operation.setKeywords("raster,selection,submap,attribute");

    mastercatalog()->addItems({operation});
    return operation.id();
}
//---------------------------------------------------
REGISTER_OPERATION(AttributeRaster)
AttributeRaster::AttributeRaster() : SelectionRaster()
{}

AttributeRaster::AttributeRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : SelectionRaster(metaid, expr){}

Ilwis::OperationImplementation *AttributeRaster::create(quint64 metaid,const Ilwis::OperationExpression& expr){
    return new AttributeRaster(metaid, expr);
}

Ilwis::OperationImplementation::State AttributeRaster::prepare(ExecutionContext *ctx, const SymbolTable& tbl){
    QString newExpression = QString("selection(%1,attributes(%2))").arg(_expression.input<QString>(0),_expression.input<QString>(1) );
    _expression = OperationExpression(newExpression);
    return SelectionRaster::prepare(ctx, tbl);
}

bool AttributeRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    return SelectionRaster::execute(ctx, symTable);
}

quint64 AttributeRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/attributeraster"});
    operation.setLongName("Attribute Raster");
    operation.setSyntax("attributeraster(coverage,attributecolumn)");
    operation.setDescription(TR("the operation select one column of the attribute table to create a new raster with only that attribute"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("input rastercoverage"),TR("input rastercoverage with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING,  TR("attribute column"),TR("name of the attribute column to use for the attribute raster; must be numeric or item domain"),OperationResource::ueCOMBO);
    operation.setOutParameterCount({1});
     operation.addValidation(0,1,"columns");
    operation.addOutParameter(0, itRASTER, TR("attribute raster"));
    operation.setKeywords("raster,selection,attribute");

    mastercatalog()->addItems({operation});
    return operation.id();
}

//---------------------------------------------------
REGISTER_OPERATION(RasterBand)
RasterBand::RasterBand() : SelectionRaster()
{}

RasterBand::RasterBand(quint64 metaid, const Ilwis::OperationExpression &expr) : SelectionRaster(metaid, expr){}

Ilwis::OperationImplementation *RasterBand::create(quint64 metaid,const Ilwis::OperationExpression& expr){
    return new RasterBand(metaid, expr);
}

Ilwis::OperationImplementation::State RasterBand::prepare(ExecutionContext *ctx, const SymbolTable& tbl){
    QString newExpression = QString("selection(%1,rasterbands(%2))").arg(_expression.input<QString>(0),_expression.input<QString>(1) );
    _expression = OperationExpression(newExpression);
    return SelectionRaster::prepare(ctx, tbl);
}

bool RasterBand::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    return SelectionRaster::execute(ctx, symTable);
}

quint64 RasterBand::createMetadata()
{
    OperationResource operation({"ilwis://operations/rasterband"});
    operation.setLongName("RasterBand");
    operation.setSyntax("rasterband(raster,stackindex)");
    operation.setDescription(TR("selects one band of a multi band raster"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER, TR("input raster"),TR("input rastercoverage with multiple bands"));
    operation.addInParameter(1,itSTRING,  TR("band(s)"),TR("index or band name(s) in the stack"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("output band"));
    operation.setKeywords("raster,selection");

    mastercatalog()->addItems({operation});
    return operation.id();
}


