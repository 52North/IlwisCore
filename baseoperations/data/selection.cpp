#include "raster.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "table.h"
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


    int keyColumn = _inputAttributeTable.isValid() ? _inputAttributeTable->columnIndex(COVERAGEKEYCOLUMN) : iUNDEF;

    std::vector<int> extraAtrrib = organizeAttributes();


    std::vector<QString> selectionBands = bands(inputRaster);
    if ( selectionBands.size() == 0) // use all
        selectionBands = inputRaster->stackDefinition().indexes();

    PixelIterator iterOut(outputRaster);

    for(QString band : selectionBands){
        PixelIterator iterIn = inputRaster->band(band, _box);

        PixelIterator iterEnd = iterIn.end();
        while(iterIn != iterEnd) {
            bool ok = true;
            double& pixValue = *iterIn;
            double matchValue = pixValue;
            if (keyColumn != iUNDEF){

            }
            for(const auto& epart : _expressionparts){
                bool partOk = epart.match(iterIn.position(), matchValue,this);
                if ( epart._andor != loNONE)
                    ok =  epart._andor == loAND ? ok && partOk : ok || partOk;
                else
                    ok &= partOk;
            }
            if ( ok){
                *iterOut = pixValue;
            }else
                *iterOut = rUNDEF;

            ++iterIn;
            ++iterOut;
        }
        // if there is an attribute table we must copy the correct attributes and records
        if ( keyColumn != iUNDEF){
            for(int recIndex=0; recIndex < _inputAttributeTable->recordCount(); ++recIndex){
                const Record& rec = _inputAttributeTable->record(recIndex);
                for(int i=0; i < extraAtrrib.size(); ++i){
                    _attTable->setCell(extraAtrrib[i], recIndex, rec.cell(extraAtrrib[i]));
                }
            }
        }
    }

    outputRaster->setAttributes(_attTable);
    QVariant value;
    value.setValue<IRasterCoverage>(outputRaster);
    ctx->setOutput(symTable, value, outputRaster->name(), itRASTER,outputRaster->resource());
    return true;


}

Ilwis::OperationImplementation *SelectionRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SelectionRaster(metaid, expr);
}


Ilwis::OperationImplementation::State SelectionRaster::prepare(ExecutionContext *, const SymbolTable &)
{
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
    int numbands = selectionBands.size() == 0 ? iUNDEF : selectionBands.size();
    _box = boundingBox(_inputObj.as<RasterCoverage>());
    bool useOldGrf = numbands == iUNDEF;
    if ( _box.isNull()){
        _box = inputRaster->size();
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
         //outputRaster->datadefRef() = _attribColumn != "" ? inputRaster->attributeTable()->columndefinition(_attribColumn).datadef()
         //                                          : outputRaster->datadefRef() = inputRaster->datadef();

         QString url = "ilwis://internalcatalog/" + outputName;
         Resource resource(url, itFLATTABLE);
         _attTable.prepare(resource);
     }


     if ( (copylist & itGEOREF) == 0) {
         Resource resource(QUrl("ilwis://internalcatalog/" + outputRaster->name() + "_grf_" + QString::number(outputRaster->id())),itGEOREF);
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
    operation.setOutParameterCount({1});
    operation.addOutParameter(0, itRASTER, TR("selection"));
    operation.setKeywords("raster, operation, selection");

    mastercatalog()->addItems({operation});
    return operation.id();
}

