#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "commandhandler.h"
#include "featureiterator.h"
#include "selectionfeatures.h"

using namespace Ilwis;
using namespace BaseOperations;

SelectionFeatures::SelectionFeatures()
{
}

SelectionFeatures::SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

SelectionFeatures::~SelectionFeatures()
{
}


bool SelectionFeatures::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IFeatureCoverage outputFC = _outputObj.get<FeatureCoverage>();
    IFeatureCoverage inputFC = _inputObj.get<FeatureCoverage>();

    SubSetAsyncFunc selection = [&](const std::vector<quint32>& subset ) -> bool {
        FeatureIterator iterIn(inputFC, subset);

        for_each(iterIn, iterIn.end(), [&](SPFeatureI& feature){
            QVariant v = feature->cell(_attribColumn);
            SPFeatureI& newFeature = outputFC->newFeatureFrom(feature);
            _attTable->record(NEW_RECORD,{newFeature->featureid(), v});

            ++iterIn;
        }
        );
        return true;
    };
    ctx->_threaded = false;
    bool ok = OperationHelperFeatures::execute(ctx,selection, inputFC, outputFC, _attTable);

    if ( ok && ctx != 0) {
        outputFC->attributeTable(_attTable);
        QVariant value;
        value.setValue<IFeatureCoverage>(outputFC);
        ctx->addOutput(symTable, value, outputFC->name(), itFEATURE,outputFC->source());
    }
    return true;
}

Ilwis::OperationImplementation *SelectionFeatures::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SelectionFeatures(metaid, expr);
}

Ilwis::OperationImplementation::State SelectionFeatures::prepare(ExecutionContext *, const SymbolTable &)
{
    IlwisTypes inputType = itFEATURE;
    QString fc = _expression.parm(0).value();
    if (!_inputObj.prepare(fc, inputType)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }
    IFeatureCoverage inputFC = _inputObj.get<FeatureCoverage>();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    selector = selector.remove('"');

    int index = selector.indexOf("box=");
    Box2D<double> box;
    if ( index != -1) {
        QString crdlist = "box(" + selector.mid(index+4) + ")";
        _box = Box3D<double>(crdlist);
        copylist |= itDOMAIN | itTABLE;
    }
    index = selector.indexOf("polygon=");
    if ( index != -1)
    {
        //TODO:
        copylist |= itDOMAIN | itTABLE;
    }
    index = selector.indexOf("attribute=");
    if ( index != -1 ) {
        if (! inputFC->attributeTable().isValid()) {
            ERROR2(ERR_NO_FOUND2,"attribute-table", "coverage");
            return sPREPAREFAILED;
        }
        _attribColumn =  selector.mid(index+10);
        copylist |= itENVELOPE;
    }

     _outputObj = OperationHelperFeatures::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->setName(outputName);

     if ( _attribColumn != "") {
         QString url = "ilwis://internalcatalog/" + outputName;
         Resource resource(url, itFLATTABLE);
         _attTable.prepare(resource);
         IDomain covdom;
         if (!covdom.prepare("count")){
             return sPREPAREFAILED;
         }
         if ( inputFC->attributeTable()->columnIndex(_attribColumn) == iUNDEF) {
             ERROR2(ERR_NOT_FOUND2, TR("column"), inputFC->attributeTable()->name());
             return sPREPAREFAILED;
         }
         _attTable->addColumn(FEATUREIDCOLUMN,covdom);
         _attTable->addColumn(_attribColumn, inputFC->attributeTable()->columndefinition(_attribColumn).datadef().domain());
         IFeatureCoverage outputFC = _outputObj.get<FeatureCoverage>();
         outputFC->attributeTable(_attTable);
     }
     if ( (_box.isValid() && !_box.isNull()) == 0) {
        //TODO: selections in features on bounding box
     }
     return sPREPARED;
}

quint64 SelectionFeatures::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","selection");
    resource.addProperty("syntax","selection(featurecoverage,selection-definition)");
    resource.addProperty("description",TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    resource.addProperty("inparameters","2");
    resource.addProperty("pin_1_type", itFEATURE);
    resource.addProperty("pin_1_name", TR("input rastercoverage"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with a domain as specified by the selection"));
    resource.addProperty("pin_2_type", itSTRING);
    resource.addProperty("pin_2_name", TR("selection-definition"));
    resource.addProperty("pin_2_desc",TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    resource.addProperty("pout_1_type", itFEATURE);
    resource.addProperty("pout_1_name", TR("rastercoverage were the selection has been applied"));
    resource.addProperty("pout_1_desc",TR(""));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();

}
