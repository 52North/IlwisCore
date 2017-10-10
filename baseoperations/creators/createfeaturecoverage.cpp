#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "itemdomain.h"
#include "itemiterator.h"
#include "factory.h"
#include "coverage.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "createfeaturecoverage.h"


using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateFeatureCoverage)

CreateFeatureCoverage::CreateFeatureCoverage()
{

}

CreateFeatureCoverage::CreateFeatureCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *CreateFeatureCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateFeatureCoverage(metaid, expr);
}

bool Ilwis::BaseOperations::CreateFeatureCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    return true;
}

void  CreateFeatureCoverage::registerColumnDefinitions(const IFeatureCoverage& fc, int stackIndex){
    const FeatureAttributeDefinition& attribs =  fc->attributeDefinitionsRef(stackIndex);
    for(int col=0; col < attribs.columnCount(); ++col){
        bool found = false;
        for(auto coldef : _attributeDefinitions[stackIndex]){
            if ( coldef.equals(attribs.columndefinition(col))){
                found = true;
                break;
            }
        }
    }
}

Ilwis::OperationImplementation::State CreateFeatureCoverage::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    QString csy = _expression.input<QString>(0);
    if (!_csy.prepare(csy)){
        kernel()->issues()->log(TR("can't open coordinate system:") + csy);
        return sPREPAREFAILED;
    }

    QString coverages = _expression.input<QString>(1);
    QString stackDomain = _expression.input<QString>(2);
    if (!_stackDomain.prepare(stackDomain)){
        kernel()->issues()->log(TR("can't open domain:") + stackDomain);
        return sPREPAREFAILED;
    }
    if ( !hasType(_stackDomain->ilwisType(), itITEMDOMAIN | itNUMBER)){
        kernel()->issues()->log(TR("not a suitable domain for stack:") + stackDomain);
        return sPREPAREFAILED;
    }
    QStringList parts = coverages.split("),");
    int stackIndex = 0;
    for(const QString& part : parts){
        int index = part.indexOf("(");
        if (  index != -1){
            QString domElement = part.left(index);
            if ( !_stackDomain->contains(domElement)){
                kernel()->issues()->log(TR("the stack domain doesn't contain the element:") + domElement);
                return sPREPAREFAILED;
            }
            QStringList fcs = part.mid(index + 1).split(",");
            for(const QString& fc : fcs){
                IFeatureCoverage featureCov;
                if (!featureCov.prepare(fc)){
                    kernel()->issues()->log(TR("can't open feature coverage:") + fc);
                    return sPREPAREFAILED;
                }
                _features[domElement] = featureCov;
                registerColumnDefinitions(featureCov, stackIndex);
            }
        }
    }
    return sPREPARED;
}

quint64 CreateFeatureCoverage::createMetadata(){

    OperationResource resource({"ilwis://operations/createfeaturecoverage"});
    resource.setLongName("Create Featurer coverage");
    resource.setSyntax("createfeaturecoverage(coordinatesystem, coveragelist,stackdomain");
    resource.setInParameterCount({3});
    resource.addInParameter(0,itCOORDSYSTEM, "Coordinate system", "Coordinatesystem used by the output feature coverage");
    resource.addInParameter(1, itSTRING,TR("Featurew coverages"), TR("A list of feature coverages to be added to the new coverage. Different stack entries may be indicated by <stack-element>(<list of coverages>)"));
    resource.addInParameter(2,itDOMAIN, "Stack domain", "domain used for the stack, by default it is count");
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itFEATURE, TR("new feature coverage"), TR("The newly createdfeature coverage"));
    resource.setKeywords("create, feature, workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
