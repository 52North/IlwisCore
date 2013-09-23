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

        AttributeRecord rec;
        if ( _attribColumn != "")
            rec = AttributeRecord(inputFC->attributeTable(itFEATURE), FEATUREIDCOLUMN);
        SPAttributeRecord attrib (new AttributeRecord(_attTable, FEATUREIDCOLUMN));
        quint64 v_in = 0;
        for_each(iterIn, iterIn.end(), [&](SPFeatureI feature){
            QVariant v;
            if ( rec.isValid()) {
                SPFeatureI feature = *iterIn;
                v_in = feature->value(FEATUREIDCOLUMN).toULongLong();
                v = rec.cellByKey(v_in,_attribColumn);
            } else {
                v = v_in;
            }
            SPFeatureI newFeature = outputFC->newFeatureFrom(feature);
            if ( !newFeature.isNull()) {
                newFeature->attributeRecord(attrib);
                _attTable->record(NEW_RECORD,{newFeature->featureid(), v});
            }

            ++iterIn;
        }
        );
        return true;
    };
    ctx->_threaded = false;
    bool res = OperationHelperFeatures::execute(ctx,selection, inputFC, outputFC, _attTable);

    if ( res && ctx != 0) {
        outputFC->attributeTable(outputFC->featureTypes(),_attTable);
        QVariant value;
        value.setValue<IFeatureCoverage>(outputFC);
        ctx->addOutput(symTable, value, outputFC->name(), itFEATURE,outputFC->resource());
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
        //TODO
        copylist |= itDOMAIN | itTABLE;
    }
    index = selector.indexOf("attribute=");
    if ( index != -1 ) {
        if (! inputFC->attributeTable(itFEATURE).isValid()) {
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
     IFeatureCoverage outputFC = _outputObj.get<FeatureCoverage>();
     if ( (copylist & itDOMAIN) == 0) {
         outputFC->datadef() = inputFC->datadef();
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->setName(outputName);

     if ( _attribColumn != "") {
         QString url = "ilwis://internal/" + outputName;
         Resource res(url, itFLATTABLE);
         _attTable.prepare(res);
         IDomain covdom;
         if (!covdom.prepare("count")){
             return sPREPAREFAILED;
         }
         _attTable->addColumn(FEATUREIDCOLUMN,covdom);
         _attTable->addColumn(_attribColumn, inputFC->attributeTable(itFEATURE)->columndefinition(_attribColumn).datadef().domain());
     }
     if ( (_box.isValid() && !_box.isNull()) == 0) {
        //TODO selections in features on bounding box
     }
     return sPREPARED;
}

quint64 SelectionFeatures::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","selection");
    res.addProperty("syntax","selection(featurecoverage,selection-definition)");
    res.addProperty("description",TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itFEATURE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with a domain as specified by the selection"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("selection-definition"));
    res.addProperty("pin_2_desc",TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    res.addProperty("pout_1_type", itFEATURE);
    res.addProperty("pout_1_name", TR("gridcoverage were the selection has been applied"));
    res.addProperty("pout_1_desc",TR(""));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();

}
