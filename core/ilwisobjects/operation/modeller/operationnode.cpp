#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "workflownode.h"
#include "operationnode.h"

using namespace Ilwis;

OperationNode::OperationNode() : WorkFlowNode("")
{

}
OperationNode::OperationNode(quint64 operationid) : WorkFlowNode("")
 {
     Resource res = mastercatalog()->id2Resource(operationid);
     IOperationMetaData op;
     op.prepare(res);
     name(op->name());
     setDescription(op->description());
     _operationid = operationid;
     std::vector<SPOperationParameter> parms = op->getInputParameters();
     QStringList required, optional;
     op->parametersFromSyntax(required, optional);
     for(int i=0; i < parms.size(); ++i){
         WorkFlowParameter wfp(i, this->id(), parms[i]->name(), parms[i]->description());
         wfp.value(sUNDEF, parms[i]->type()); // empty value be we know its type
         if ( i < required.size())
             wfp.addSyntax(required[i]);
         else
             wfp.addSyntax(optional[i - required.size()]);
         addInput(wfp);
     }
}

OperationNode::OperationNode(const QString &name, const QString &description, quint64 id) : WorkFlowNode(name, description, id)
{

}

bool OperationNode::execute(ExecutionContext *ctx, SymbolTable &symTable, const OperationExpression& expression, const std::map<quint64, int>& idmap)
{
    auto getValue = [&](const WorkFlowParameter& parm, const OperationExpression& expression, const std::map<quint64, int>& idmap)-> QVariant{
        auto iter = idmap.find(parm.id());
        if  ( iter != idmap.end())       {
            int idx = (*iter).second;
            return expression.parm(idx).value();
        }
        return parm.value();
    };
    SymbolTable symTable2(symTable);
    for(WorkFlowParameter& parameter : _inputParameters1){ // check if all parameters are known
        bool hasValue = parameter.value() != sUNDEF; // we only need to go up the excecution tree if the value is not set yet
        if ( !hasValue){
            ExecutionContext ctx2;
            if (parameter.inputLink()) {
                if ( parameter.inputLink()->execute(&ctx2, symTable2, expression, idmap)) {
                    QString outputName = ctx2._results[parameter.outputParameterIndex()];
                    QVariant val = symTable2.getValue(outputName);
                    QString sval = OperationHelper::variant2string(val, symTable2.getSymbol(outputName)._type);
                    parameter.value(sval, symTable2.getSymbol(outputName)._type);
                }else
                    return false;
            }
        }
    }
    IOperationMetaData metadata = operation();
    QString expr = metadata->name()  + "(";
    QString parms;
    for(WorkFlowParameter& inParam : _inputParameters1){
        if ( parms != "")
            parms += ",";
        if ( hasType(inParam.valueType(),itILWISOBJECT)){
            parms += getValue(inParam, expression, idmap).toString();
        }else if ( hasType(inParam.valueType(),itINTEGER )) {
            parms += QString::number(getValue(inParam,expression, idmap).toLongLong());
        } else if ( hasType(inParam.valueType(),itDOUBLE | itFLOAT)) {
            parms += QString::number(getValue(inParam,expression, idmap).toDouble());
        } else if (hasType(inParam.valueType(),itSTRING)){
            parms += "'" + getValue(inParam,expression, idmap).toString() + "'";
        }else
            parms += getValue(inParam,expression, idmap).toString();
    }
  // expr = "script " + outNames +  "=" + expr + parms + ")";
    expr = expr + parms + ")";
    bool ok = commandhandler()->execute(expr,ctx,symTable2);
    symTable.copyFrom(ctx, symTable2);

    return ok;

}

IOperationMetaData OperationNode::operation() const
{
    Resource resource = mastercatalog()->id2Resource(_operationid);
    IOperationMetaData metadata;
    metadata.prepare(resource);
    return metadata;
}

quint64 OperationNode::operationid() const
{
    return _operationid;
}

void OperationNode::operationid(quint64 iod)
{
    _operationid = iod;
}

QString OperationNode::type() const
{
    return "operationnode";
}

bool OperationNode::isValid(Workflow *, WorkFlowNode::ValidityCheck vc) const
{
    if ( vc == WorkFlowNode::vcPARTIAL)
        return true;
    if ( vc == WorkFlowNode::vcALLDEFINED){
        bool ok = true;
        for(int i=0; i < inputCount() && ok; ++i){
            ok &= input(i).state() != WorkFlowParameter::pkFREE;
        }
        return ok;
    }
    return false;
}
