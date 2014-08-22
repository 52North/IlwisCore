#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "commandhandler.h"
#include "featureiterator.h"
#include "selectionfeatures.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SelectionFeatures)

SelectionFeatures::SelectionFeatures()
{
}

SelectionFeatures::SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

SelectionFeatures::~SelectionFeatures()
{
}

bool SelectionFeatures::createIndexes(const IFeatureCoverage& inputFC, ExecutionContext *ctx, SymbolTable &symTable){
    std::set<quint32> resultset;

    quint32 index = 0;
    for(const auto& feature : inputFC){
        QVariant val = feature(_attribColumn);
        if ( QString(val.typeName()) == "QString" && QString(_rightSide.typeName()) == "QString"){
            if ( compare1(_operator,val.toString(), _rightSide.toString()))
                resultset.insert(index);
        } else {
            bool ok1,ok2;
            double v1 = val.toDouble(&ok1);
            double v2 = _rightSide.toDouble(&ok2);
            bool ok3 = compare1(_operator, v1, v2);
            if ( ok1&& ok2 && ok3)
                resultset.insert(index);

        }
        ++index;

    }
    Indices result(resultset.begin(), resultset.end());
    if ( ctx != 0) {
        QVariant value;
        value.setValue<Indices>(result);
        ctx->addOutput(symTable, value, sUNDEF, itINTEGER | itCOLLECTION, Resource());
    }

    return true;
}

bool SelectionFeatures::createCoverage(const IFeatureCoverage& inputFC, ExecutionContext *ctx, SymbolTable &symTable)
{
    IFeatureCoverage outputFC = _outputObj.as<FeatureCoverage>();

    SubSetAsyncFunc selection = [&](const std::vector<quint32>& subset ) -> bool {
        FeatureIterator iterIn(inputFC, subset);

        for_each(iterIn, iterIn.end(), [&](UPFeatureI& feature){
            QVariant v = feature->cell(_attribColumn);
            UPFeatureI& newFeature = outputFC->newFeatureFrom(feature);
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
        ctx->setOutput(symTable, value, outputFC->name(), itFEATURE,outputFC->source());
    }
    return ok;
}

bool SelectionFeatures::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

   IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();

    if ( _asIndex)
        return createIndexes(inputFC, ctx, symTable);
    else
        return createCoverage(inputFC, ctx, symTable);
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
    IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    selector = selector.remove('"');

    int index = selector.indexOf("box=");
    Envelope box;
    if ( index != -1) {
        QString crdlist = "box(" + selector.mid(index+4) + ")";
        _box = Envelope(crdlist);
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
        std::map<QString,LogicalOperator> operators = {{"==", loEQ},{"<=",loLESSEQ},{">=", loGREATEREQ},{"<",loLESS},{">", loGREATER},{"!=",loNEQ}};
        int index1 = selector.indexOf("\"");
        if ( index1 == -1)
            index1 = 10000;
        int index2 = 0;
        for(auto op : operators){
            if ((index2 = selector.indexOf(op.first)) != -1){
                if ( index2 < index1)    {
                    _operator = op.second;
                    index = selector.indexOf("=");
                    _attribColumn = selector.mid(index + 1, index2 - index - 1).trimmed();
                    _rightSide = selector.mid(index2 + op.first.size()).trimmed();
                    break;
                }
            }
        }
        if (_attribColumn.size() == 0)
            _attribColumn =  selector.mid(index+10);
        copylist |= itENVELOPE;
    }
    if ( _expression.parameterCount() == 3){
        _asIndex = _expression.parm(2).value().toLower() == "asindex";
    }

     _outputObj = OperationHelperFeatures::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->name(outputName);

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
         _attTable->addColumn(_attribColumn, inputFC->attributeTable()->columndefinition(_attribColumn).datadef().domain<>());
         IFeatureCoverage outputFC = _outputObj.as<FeatureCoverage>();
         outputFC->attributeTable(_attTable);
     }
     if ( (_box.isValid() && !_box.isNull()) == 0) {
        //TODO: selections in features on bounding box
     }
     return sPREPARED;
}

quint64 SelectionFeatures::createMetadata()
{
    OperationResource operation({"ilwis://operations/selection"});
    operation.setSyntax("selection(featurecoverage,selection-definition[,asIndex])");
    operation.setDescription(TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itFEATURE, TR("input feature coverage"),TR("input feature rcoverage with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING, TR("selection-definition"),TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    operation.addInParameter(2,itSTRING, TR("alternate return type"),TR("optional, if asIndex is used the result will be a selection of feature indexes in the coverage"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE | itCOLLECTION, TR("Selection"),TR("coverage or index were the selection has been applied") );
    operation.setKeywords("feature, selection");

    mastercatalog()->addItems({operation});
    return operation.id();

}
