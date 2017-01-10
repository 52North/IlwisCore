#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "table.h"
#include "raster.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "workflowimplementation.h"

using namespace Ilwis;

WorkflowImplementation::WorkflowImplementation()
{

}

WorkflowImplementation::WorkflowImplementation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

template<class T> void setOutput(const QVariant& value,ExecutionContext *ctx, SymbolTable &symTable){
    T obj = value.value<T>();
    ctx->addOutput(symTable,value,obj->name(), obj->ilwisType(), obj->resource() );
}

bool WorkflowImplementation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    ExecutionContext ctx2;
    SymbolTable symTable2;
    std::vector<SPWorkFlowNode> nodes = _workflow->outputNodes();
    for(SPWorkFlowNode node : nodes ) {
        node->execute(&ctx2, symTable2, _expression, _workflow->parmid2order());
    }
    for(int i=0; i < ctx2._results.size(); ++i){

        Symbol sym = symTable2.getSymbol(ctx2._results[i]);
        QVariant value = sym._var;
        if ( hasType(sym._type, itTABLE)){
            setOutput<ITable>(value, ctx, symTable);
        }else  if ( hasType(sym._type, itRASTER)){
            setOutput<IRasterCoverage>(value, ctx, symTable)   ;
        }else if ( hasType(sym._type, itFEATURE)){
            setOutput<IFeatureCoverage>(value, ctx, symTable)   ;
        }else if ( hasType(sym._type, itNUMBER)){
             ctx->addOutput(symTable, QVariant(value), sUNDEF, itDOUBLE, Resource());
        }else if ( hasType(sym._type, itSTRING)){
            ctx->addOutput(symTable, QVariant(value), sUNDEF, itSTRING, Resource());
       }
    }

    return true;
}

Ilwis::OperationImplementation *WorkflowImplementation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new WorkflowImplementation(metaid, expr);
}

Ilwis::OperationImplementation::State WorkflowImplementation::prepare(ExecutionContext *ctx, const SymbolTable &)
{

    QString url = "ilwis://operations/" + _expression.name() + ".ilwis";
    quint64 id = mastercatalog()->name2id(url, itWORKFLOW);
    if ( id == i64UNDEF){
        kernel()->issues()->log(TR("Couldnt find a valid workflow for ") + _expression.name());
        return sPREPAREFAILED;
    }
    if (!_workflow.prepare(id)){
        kernel()->issues()->log(TR("Couldnt find a valid workflow for ") + _expression.name());
        return sPREPAREFAILED;
    }
    return sPREPARED;
}



