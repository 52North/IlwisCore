#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createthematicdomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateThematicDomain)

CreateThematicDomain::CreateThematicDomain()
{

}

CreateThematicDomain::CreateThematicDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateThematicDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IThematicDomain thematicdomain;
    thematicdomain.prepare();
    thematicdomain->setDescription(_domaindesc);

    if ( _parentdomain.isValid())
        thematicdomain->setParent(_parentdomain);
    for(int i=0; i < _items.size(); i+=3){
        thematicdomain->addItem(new ThematicItem(_items[i], _items[i+1],_items[i+2]));
    }

    QVariant value;
    value.setValue<IDomain>(thematicdomain);
    logOperation(thematicdomain, _expression);
    ctx->setOutput(symTable,value,thematicdomain->name(),itDOMAIN,thematicdomain->resource());

    return true;
}

Ilwis::OperationImplementation *CreateThematicDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateThematicDomain(metaid, expr);
}

Ilwis::OperationImplementation::State CreateThematicDomain::prepare(ExecutionContext *ctx, const SymbolTable &st)
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
    if ( !_parentdomain.isValid()) // each item must contain name/code/description in the case of non parented domains
        _items = items.split("|");
    else { // in the case there is a parent domain only item names are expected as the rest of the info is already in the parent. still the list must be complete as the execute expects it; empty code/description are sufficient though
        QStringList shortlist = items.split("|");
        for(auto item : shortlist){
            _items.push_back(item);
            _items.push_back("");
            _items.push_back("");
        }
    }
    if (_items.size() % 3 != 0) {
        kernel()->issues()->log(QString(TR("%1 item definition is not correct; do all items have name, code(may be empty), description (may be empty)?")).arg(_expression.input<QString>(3)));
        return sPREPAREFAILED;
    }
    if ( _parentdomain.isValid()){
        for(int i =0; i < _items.size(); i+=3){
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

quint64 CreateThematicDomain::createMetadata()
{
    OperationResource resource({"ilwis://operations/createthematicdomain"});
    resource.setLongName("Create Thematic Domain");
    resource.setSyntax("createthematicdomain(itemdefintion, strict,description[,parentdomain])");
    resource.setInParameterCount({3,4});
    resource.addInParameter(0, itSTRING,TR("Item defintion"), TR("A '|' seperate list of item definition for the items of the domain, each item consist always of 3 parts, the last 2 parts maybe empty"));
    resource.addInParameter(1, itBOOL,TR("Strictness"), TR("Indicates how strict the realtion between domain and parent is; true for all domains without parents"));
    resource.addInParameter(2, itSTRING,TR("Description"), TR("Extra information about the new domain"));
    resource.addOptionalInParameter(3, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itITEMDOMAIN, TR("output domain"), TR("The newly created domain"));
    resource.setKeywords("domain, create, thematic, itemdomain,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
