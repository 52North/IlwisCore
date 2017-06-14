#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "combinationmatrix.h"
#include "createcombinationmatrix.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateCombinationMatrix)

CreateCombinationMatrix::CreateCombinationMatrix()
{

}

CreateCombinationMatrix::CreateCombinationMatrix(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateCombinationMatrix::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    ICombinationMatrix matrix;

    Resource res(itCOMBINATIONMATRIX);
    res.prepare();
    matrix.prepare(res);
    matrix->axisDefinition(CombinationMatrix::aXAXIS, {_xaxisDomain});
    matrix->axisDefinition(CombinationMatrix::aYAXIS, {_yaxisDomain});
    matrix->combinationDef({_comboDomain});
    std::vector<QString> values;
    std::copy(_xAxisValues.begin(), _xAxisValues.end(), std::back_inserter(values));
    matrix->axisValues(CombinationMatrix::aXAXIS, values);
    values = std::vector<QString>();
    std::copy(_yAxisValues.begin(), _yAxisValues.end(), std::back_inserter(values));
    matrix->axisValues(CombinationMatrix::aYAXIS, values);

    for(int y=0; y < _yAxisValues.size(); ++y){
        for(int x=0; x < _xAxisValues.size(); ++x){
            matrix->combo(x,y,_values[x +  _yAxisValues.size()*y]);
        }
    }
    QVariant value;
    value.setValue<>(matrix);
    logOperation(matrix, _expression);
    ctx->setOutput(symTable,value,matrix->name(),itCOMBINATIONMATRIX,matrix->resource());

    return true;
}

Ilwis::OperationImplementation *CreateCombinationMatrix::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateCombinationMatrix(metaid, expr);
}

Ilwis::OperationImplementation::State CreateCombinationMatrix::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);

    QString domainName = _expression.input<QString>(0);
    if (! _xaxisDomain.prepare(domainName)){
        kernel()->issues()->log(TR("Domain doesnt exist :") + domainName);
        return sPREPAREFAILED;
    }

    domainName = _expression.input<QString>(1);
    if (! _yaxisDomain.prepare(domainName)){
        kernel()->issues()->log(TR("Domain doesnt exist :") + domainName);
        return sPREPAREFAILED;
    }

    domainName = _expression.input<QString>(2);
    if (! _comboDomain.prepare(domainName)){
        kernel()->issues()->log(TR("Domain doesnt exist :") + domainName);
        return sPREPAREFAILED;
    }
    _xAxisValues = _expression.input<QString>(3).split("|");
    for(auto item : _xAxisValues){
        if ( item == sUNDEF)
            continue;
        if(!_xaxisDomain->contains(item)){
            kernel()->issues()->log(TR("Item doesn exist in x axis domain:") + item);
            return sPREPAREFAILED;
        }
    }
    _yAxisValues = _expression.input<QString>(4).split("|");
    for(auto item : _yAxisValues){
        if ( item == sUNDEF)
            continue;
        if(!_yaxisDomain->contains(item)){
            kernel()->issues()->log(TR("Item doesn exist in y axis domain:") + item);
            return sPREPAREFAILED;
        }
    }
    QStringList items = _expression.input<QString>(5).split("|");
     _values.resize(_xAxisValues.size() * _yAxisValues.size(), rUNDEF);

    for(int i=0; i < items.size(); i+=3){
        QString item = items[i+2];
        if ( item == sUNDEF)
            continue;

        if(!_comboDomain->contains(item)){
            continue;
        }
        for(int j=0; j < _xAxisValues.size(); ++j){
            if ( _xAxisValues[j] == items[i])   {
                for(int k=0;k < _yAxisValues.size(); ++k){
                    if ( _yAxisValues[k] == items[i+1])   {
                        _values[k * _yAxisValues.size() + j] = _comboDomain->impliedValue(item).toDouble();
                        k = j = 100000;
                    }
                }
            }
        }
    }

    return sPREPARED;
}

quint64 CreateCombinationMatrix::createMetadata()
{
    OperationResource resource({"ilwis://operations/createcombinationmatrix"});
    resource.setLongName("Create Combination Matrix");
    resource.setSyntax("createcombinationmatrix(domainxaxis,domainyaxis,domaincombos, xaxisvalues,yaxisvalues,combovalues)");
    resource.setInParameterCount({7});
    resource.addInParameter(0, itDOMAIN,TR("X Axis Domain"), TR("Domain used for the values on the x-axis"));
    resource.addInParameter(1, itDOMAIN,TR("Y Axis Domain"), TR("Domain used for the values on the y-axis"));
    resource.addInParameter(2, itDOMAIN,TR("Combo Domain"), TR("Domain used for the combinations of the values on x and y axis"));
    resource.addInParameter(3, itSTRING,TR("X Axis values"), TR("The values on X axis, a list seperated by '|' "));
    resource.addInParameter(4, itSTRING,TR("Y Axis values"), TR("The values on Y axis, a list seperated by '|' "));
    resource.addInParameter(5, itSTRING,TR("Combo values"), TR("The values of the combo cells organized in rows, a list seperated by '|' "));
    resource.addInParameter(6, itSTRING,TR("Description"), TR("Short text explaining the use of the this combination matrix"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itCOMBINATIONMATRIX, TR("output matrix"), TR("The newly created combination matrix"));
    resource.setKeywords("classification, table,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
