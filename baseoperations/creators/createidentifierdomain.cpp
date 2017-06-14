#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createidentifierdomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateIdentifierDomain)

CreateIdentifierDomain::CreateIdentifierDomain()
{

}

CreateIdentifierDomain::CreateIdentifierDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateIdentifierDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    INamedIdDomain nameiddomain;
    nameiddomain.prepare();
    nameiddomain->setDescription(_domaindesc);

    if ( _parentdomain.isValid())
        nameiddomain->setParent(_parentdomain);
    for(int i=0; i < _items.size();++i){
        nameiddomain->addItem(new NamedIdentifier(_items[i]));
    }

    QVariant value;
    value.setValue<IDomain>(nameiddomain);
    logOperation(nameiddomain, _expression);
    ctx->setOutput(symTable,value,nameiddomain->name(),itDOMAIN,nameiddomain->resource());

    return true;
}

Ilwis::OperationImplementation *CreateIdentifierDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateIdentifierDomain(metaid, expr);
}

Ilwis::OperationImplementation::State CreateIdentifierDomain::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() == 4){
        _parentdomain.prepare(_expression.input<QString>(3),{"mustexist", true});
        if (!_parentdomain.isValid()){
            kernel()->issues()->log(QString(TR("%1 is not a valid domain")).arg(_expression.input<QString>(3)));
            return sPREPAREFAILED;
        }
        if (_parentdomain->valueType() != itTHEMATICITEM){
            kernel()->issues()->log(QString(TR("%1 is not of the correct domain type")).arg(_expression.input<QString>(3)));
            return sPREPAREFAILED;
        }
    }
    QString items = _expression.input<QString>(0);
    items.remove("\"");
    _items = items.split("|");
     if ( _parentdomain.isValid()){
        for(int i =0; i < _items.size(); ++i){
            if(!_parentdomain->contains({_items[i]})){
                kernel()->issues()->log(QString(TR("%1 is not contained in parent domain and thus not allowed")).arg(_items[i]));
                return sPREPAREFAILED;
            }

        }
    }
    _strict = _expression.input<bool>(1);
    _domaindesc = _expression.input<QString>(2);
    return sPREPARED;
}

quint64 CreateIdentifierDomain::createMetadata()
{
    OperationResource resource({"ilwis://operations/createidentifierdomain"});
    resource.setLongName("Create Identifier Domain");
    resource.setSyntax("createidentifierdomain(itemdefintion, strict,description[,parentdomain])");
    resource.setInParameterCount({3,4});
    resource.addInParameter(0, itSTRING,TR("Item defintion"), TR("A '|' seperate list of item definition for the items of the domain"));
    resource.addInParameter(1, itBOOL,TR("Strictness"), TR("Indicates how strict the realtion between domain and parent is; true for all domains without parents"));
    resource.addInParameter(2, itSTRING,TR("Description"), TR("Extra information about the new domain"));
    resource.addOptionalInParameter(3, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itITEMDOMAIN, TR("output domain"), TR("The newly created domain"));
    resource.setKeywords("domain, create, identifier, itemdomain,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

