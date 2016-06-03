#include <QString>
#include <QRegularExpression>
#include <functional>
#include <future>
#include <memory>
#include "geos/geom/Envelope.inl"
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
#include "featureiterator.h"
#include "geometryhelper.h"
#include "selectionfeatures.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SelectionFeatures)

SelectionFeatures::ExpressionPart::ExpressionPart(const ITable& attributes, const QString& part){
    _isValid = false;
    int index = part.indexOf("box(");
    if ( index != -1) {
        _envelope = part;
        _isValid = _envelope.isValid();
        _type = ptBOX;
    }else {
        index = part.indexOf("polygon(");
        if ( index != -1)
        {
            _polygon.reset(GeometryHelper::fromWKT(part,0));
            _isValid = _polygon.get() != 0;
            _type = ptPOLYGON;

        }else {
            index = part.indexOf("attribute=");
            if ( index != -1) {
                QStringList attribs = part.split(",");
                for(auto attrib : attribs){
                    if ( attributes->columnIndex(attrib) != iUNDEF)    {
                        _attributes.push_back(attrib);
                    }
                }
                _type = ptATTRIBUTE;
            }else {
                std::map<QString,LogicalOperator> operators = {{"==", loEQ},{"<=",loLESSEQ},{">=", loGREATEREQ},{"<",loLESS},{">", loGREATER},{"!=",loNEQ}};
                int index1 = part.indexOf("\"");
                if ( index1 == -1)
                    index1 = 10000;
                int index2 = 0;
                for(auto op : operators){
                    if ((index2 = part.indexOf(op.first)) != -1){
                        if ( index2 < index1)    {
                            _operator = op.second;
                            _leftSide = part.left(index2).trimmed();
                            if ( attributes->columnIndex(_leftSide) == iUNDEF){
                                _isValid = false;
                            }
                            _rightSide = part.mid(index2 + op.first.size()).trimmed();
                            _type = ptATTRIBUTESELECTION;
                            break;
                        }
                    }
                }
            }
        }
    }
}


bool SelectionFeatures::ExpressionPart::match(const SPFeatureI &feature,SelectionFeatures *operation) const
{
    if ( _type == ExpressionPart::ptBOX && _envelope.isValid())   {
        return _envelope.contains(feature->geometry().get());
    }
    if ( _type == ExpressionPart::ptPOLYGON && _polygon.get() != 0)   {
        return _polygon->contains(feature->geometry().get());
    }
    if ( _type == ExpressionPart::ptATTRIBUTESELECTION )   {
        QVariant val = feature(_leftSide);
        if ( QString(val.typeName()) == "QString" && QString(_rightSide.typeName()) == "QString"){
            return operation->compare1(_operator,val.toString(), _rightSide.toString());
        } else {
            bool ok1,ok2;
            double v1 = val.toDouble(&ok1);
            double v2 = _rightSide.toDouble(&ok2);
            bool ok3 = operation->compare1(_operator, v1, v2);
            return ok1&& ok2 && ok3;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------------------------

//
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

   IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();

   IFeatureCoverage outputFC = _outputObj.as<FeatureCoverage>();

   std::vector<int> extraAtrrib;
   for(const auto& epart : _expressionparts){
       if ( epart._type != ExpressionPart::ptATTRIBUTE){
           for(int i=0; i < epart._attributes.size();++i){
               int index = inputFC->attributeDefinitions().columnIndex(epart._attributes[i]);
               if ( index != iUNDEF){
                   extraAtrrib.push_back(index);
               }
           }
       }
   }
   if ( extraAtrrib.size() == 0){
       for(int c = 0; c < inputFC->attributeDefinitions().columnCount(); ++c){
           extraAtrrib.push_back(c);
       }
   }
   for(int c = 0; c < extraAtrrib.size(); ++c){
       _attTable->addColumn( inputFC->attributeDefinitions().columndefinition(extraAtrrib[c]));
   }
   int rec = 0;
   Envelope env;
   for(const auto& feature : inputFC){
       bool ok = true;
       for(const auto& epart : _expressionparts){
           if ( epart._type != ExpressionPart::ptATTRIBUTE){
               bool partOk =  epart.match(feature, this);
               if ( epart._andor != loNONE)
                   ok =  epart._andor == loAND ? ok && partOk : ok || partOk;
               else
                   ok = partOk;
           }
       }
       if ( ok){
           SPFeatureI newFeature = outputFC->newFeatureFrom(feature);
           env += Envelope(*feature->geometry()->getEnvelopeInternal());
           for(int i=0; i < extraAtrrib.size(); ++i){
               _attTable->setCell(extraAtrrib[i], rec, feature->cell(extraAtrrib[i]));
           }
           ++rec;
       }
   }

   outputFC->setAttributes(_attTable);
   outputFC->envelope(env);

   QVariant value;
   value.setValue<IFeatureCoverage>(outputFC);
   ctx->setOutput(symTable, value, outputFC->name(), itFEATURE,outputFC->resource());
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
    IFeatureCoverage inputFC = _inputObj.as<FeatureCoverage>();
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    selector = selector.remove('"');

    QRegularExpression re("( and )|( or )");
    QStringList parts = selector.split(re);

    int lastIndex = 0;
    for(QString& part : parts){
        int index = selector.indexOf(part, lastIndex);
        ExpressionPart epart(inputFC->attributeTable(), part);
        if ( index != lastIndex){
            QString logical = selector.mid(lastIndex,index - lastIndex);
            logical = logical.trimmed().toLower();
            if ( logical == "and")
                epart._andor = loAND;
            if ( logical == "or")
                epart._andor = loOR;
        }else{
            ExpressionPart epart(inputFC->attributeTable(), part);
        }
        lastIndex += part.size();

        _expressionparts.push_back(epart);
    }

     _outputObj = OperationHelperFeatures::initialize(_inputObj,inputType, copylist);
     if ( !_outputObj.isValid()) {
         ERROR1(ERR_NO_INITIALIZED_1, "output coverage");
         return sPREPAREFAILED;
     }
     QString outputName = _expression.parm(0,false).value();
     if ( outputName != sUNDEF)
         _outputObj->name(outputName);

     QString url = "ilwis://internalcatalog/" + outputName;
     Resource resource(url, itFLATTABLE);
     _attTable.prepare(resource);

     return sPREPARED;
}

quint64 SelectionFeatures::createMetadata()
{
    OperationResource operation({"ilwis://operations/selection"});
    operation.setSyntax("selection(featurecoverage,selection-definition[,asIndex])");
    operation.setLongName("Select features");
    operation.setDescription(TR("the operation select parts of the spatial extent or attributes to create a 'smaller' coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE, TR("input feature coverage"),TR("input feature rcoverage with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING, TR("selection-definition"),TR("Selection can either be attribute, layer index or area definition (e.g. box)"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE | itCOLLECTION, TR("Selection"),TR("coverage or index were the selection has been applied") );
    operation.setKeywords("feature, selection");

    mastercatalog()->addItems({operation});
    return operation.id();

}
