#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createnumericdomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateNumericDomain)

CreateNumericDomain::CreateNumericDomain()
{

}

CreateNumericDomain::CreateNumericDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateNumericDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    IIlwisObject obj;
    Resource res = Resource(itNUMERICDOMAIN);
    res.prepare();
    obj.prepare(res);
    obj->setDescription(_domaindesc);
    INumericDomain numdom = obj.as<NumericDomain>();

    NumericRange numrange(_vmin,_vmax,_vres);
    numdom->range(numrange.clone());
    if ( _parent.isValid())
        numdom->setParent(_parent);

    QVariant value;
    value.setValue<IDomain>(numdom);
    logOperation(numdom, _expression);
    ctx->setOutput(symTable,value,numdom->name(),itDOMAIN,numdom->resource());

    return true;
}

Ilwis::OperationImplementation *CreateNumericDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateNumericDomain(metaid, expr);
}

Ilwis::OperationImplementation::State CreateNumericDomain::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    bool ok1, ok2, ok3;
    _vmin = _expression.input<double>(0, ok1);
    _vmax = _expression.input<double>(1, ok2);
    _vres = _expression.input<double>(2, ok3);

    if ((!ok1 || !ok2 || !ok3) || _vmin > _vmax || _vres < 0){
        kernel()->issues()->log(QString(TR("incompatible values in numeric domain creation : %1, %2, %3").arg(_expression.input<QString>(0), _expression.input<QString>(1), _expression.input<QString>(2))));
        return sPREPAREFAILED;
    }
    _strict = _expression.input<bool>(3);
    _domaindesc = _expression.input<QString>(4);
    if ( _expression.parameterCount() == 6){
        QString parentdom = _expression.input<QString>(5);
        if ( parentdom != ""){
            IDomain dom;
            if ( dom.prepare(parentdom,{"mustexist",true})){
                if ( dom->ilwisType() == itNUMERICDOMAIN){
                    auto rng = dom->range<Ilwis::NumericRange>();
                    if ( _vmin < rng->min() || _vmax > rng->max()){
                        kernel()->issues()->log(QString(TR("incompatible domain definition with parent domain, ranges incorrect")));
                        return sPREPAREFAILED;                        ;
                    }
                    _parent  = dom;
                }
            }
        }
    }
    return sPREPARED;
}

quint64 CreateNumericDomain::createMetadata()
{
    OperationResource resource({"ilwis://operations/createnumericdomain"});
    resource.setLongName("Create Numeric Domain");
    resource.setSyntax("createnumericdomain(min,max,resolution, strict,description[,parentdomain]))");
    resource.setInParameterCount({5,6});
    resource.addInParameter(0, itDOUBLE,TR("Minimum value"), TR("Lowest values in the numeric range"));
    resource.addInParameter(1, itDOUBLE,TR("Maximum value"), TR("Highest values in the numeric range"));
    resource.addInParameter(2, itDOUBLE,TR("Resolution"), TR("Numerical space between the numbers in the range"));
    resource.addInParameter(3, itBOOL,TR("Strictness"), TR("Indicates how strict the realtion between domain and parent is; true for all domains without parents"));
    resource.addInParameter(4, itSTRING,TR("Description"), TR("Extra information about the new domain"));
    resource.addOptionalInParameter(5, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itNUMERICDOMAIN, TR("output domain"), TR("The newly created domain"));
    resource.setKeywords("domain, create, numeric,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
