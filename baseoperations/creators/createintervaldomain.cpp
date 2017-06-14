#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "interval.h"
#include "intervalrange.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createintervaldomain.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateIntervalDomain)

CreateIntervalDomain::CreateIntervalDomain()
{

}

CreateIntervalDomain::CreateIntervalDomain(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateIntervalDomain::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IIntervalDomain intervaldomain;
    intervaldomain.prepare();
    intervaldomain->setDescription(_domaindesc);

    if ( _parentdomain.isValid())
        intervaldomain->setParent(_parentdomain);
    for(int i=0; i < _items.size(); i+=5){
        NumericRange rng(_items[i+1].toDouble(), _items[i+2].toDouble(),_resolution);
        auto *interval = new Interval(_items[i], rng);
        interval->description(_items[i+4]);
        interval->code(_items[i+3]);
        intervaldomain->addItem(interval);

    }

    QVariant value;
    value.setValue<IDomain>(intervaldomain);
    logOperation(intervaldomain, _expression);
    ctx->setOutput(symTable,value,intervaldomain->name(),itDOMAIN,intervaldomain->resource());

    return true;
}

Ilwis::OperationImplementation *CreateIntervalDomain::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateIntervalDomain(metaid, expr);
}

Ilwis::OperationImplementation::State CreateIntervalDomain::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() == 5){
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
    if ( !_parentdomain.isValid()){ // each item must contain name/code/description in the case of non parented domains
        _items = items.split("|");

    }else { // in the case there is a parent domain only item names are expected as the rest of the info is already in the parent. still the list must be complete as the execute expects it; empty code/description are sufficient though
        QStringList shortlist = items.split("|");
        for(auto item : shortlist){
            _items.push_back(item);
            _items.push_back(""); //code
            _items.push_back(""); //desc
            _items.push_back(""); //min
            _items.push_back(""); //max
        }
    }
    if (_items.size() % 5 != 0) {
        kernel()->issues()->log(QString(TR("%1 item definition is not correct; do all items have name, code(may be empty), description (may be empty)?")).arg(_expression.input<QString>(3)));
        return sPREPAREFAILED;
    }
    //check if no ranges overlap
    double last = rUNDEF;
    for(int i=0; i < _items.size(); i+=5){
       double currentMin = _items[i+1].toDouble();
       double currentMax = _items[i+2].toDouble();
       if ( last != rUNDEF && currentMax > last){
           kernel()->issues()->log(TR("Domain items overlap which is not allowed"));
           return sPREPAREFAILED;
       }
       last = currentMin;
    }
    if ( _parentdomain.isValid()){
        for(int i =0; i < _items.size(); i+=5){
            if(!_parentdomain->contains({_items[i]})){
                kernel()->issues()->log(QString(TR("%1 is not contained in parent domain and thus not allowed")).arg(_items[i]));
                return sPREPAREFAILED;
            }

        }
    }
    _resolution = _expression.input<double>(1);
    _strict = _expression.input<bool>(2);
    _domaindesc = _expression.input<QString>(3);

    return sPREPARED;
}

quint64 CreateIntervalDomain::createMetadata()
{
    OperationResource resource({"ilwis://operations/createintervaldomain"});
    resource.setLongName("Create Interval Domain");
    resource.setSyntax("createintervaldomain(intervaldefinition, resolution, strict,description[,parentdomain])");
    resource.setInParameterCount({4,5});
    resource.addInParameter(0, itSTRING,TR("Item defintion"), TR("A '|' seperate list of item definition for the items of the domain, each item consist always of 3 parts, the last 2 parts maybe empty"));
    resource.addInParameter(1, itDOUBLE,TR("Resolution"), TR("The resolution of numbers in the intervals used. All intervals have the same resolution "));
    resource.addInParameter(2, itBOOL,TR("Strictness"), TR("Indicates how strict the realtion between domain and parent is; true for all domains without parents"));
    resource.addInParameter(3, itSTRING,TR("Description"), TR("Extra information about the new domain"));
    resource.addOptionalInParameter(4, itDOMAIN, TR("Parent domain"), TR("optional indication of a parent domain"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itITEMDOMAIN, TR("output domain"), TR("The newly created domain"));
    resource.setKeywords("domain, create, interval, itemdomain,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

