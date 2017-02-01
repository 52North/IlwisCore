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
#include "workflow.h"
#include "operationnode.h"

using namespace Ilwis;

OperationNode::OperationNode() : WorkFlowNode("")
{

}
OperationNode::OperationNode(quint64 operationid) : WorkFlowNode("")
 {
     Resource res = mastercatalog()->id2Resource(operationid);
     _operation.prepare(res);
     name(_operation->name());
     setDescription(_operation->description());
     std::vector<SPOperationParameter> parms = _operation->getInputParameters();
     QStringList required, optional;
     _operation->parametersFromSyntax(required, optional);
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
            parms += "\"" + getValue(inParam,expression, idmap).toString() + "\"";
        }else
            parms += getValue(inParam,expression, idmap).toString();
    }
  // expr = "script " + outNames +  "=" + expr + parms + ")";
    expr = expr + parms + ")";
    qDebug() << "executing :" << expr;
    bool ok = commandhandler()->execute(expr,ctx,symTable2);
    symTable.copyFrom(ctx, symTable2);

    return ok;

}

IOperationMetaData OperationNode::operation() const
{
    return _operation;
}

void OperationNode::operation(const QString& provider, const QString& syntax)
{
    std::vector<Resource> items = mastercatalog()->select("catalogitemproperties.propertyname='syntax' and catalogitemproperties.propertyvalue='" + syntax + "'");
    if ( items.size() >= 1){
        _operation.prepare(items[0]);

    }
}

WorkFlowNode::NodeTypes OperationNode::type() const
{
    return WorkFlowNode::ntOPERATION;
}

bool OperationNode::isValid(const Workflow *workflow, WorkFlowNode::ValidityCheck vc) const
{
    if ( vc == WorkFlowNode::vcPARTIAL || vc == WorkFlowNode::vcAGGREGATE)
        return true;
    if ( vc == WorkFlowNode::vcALLDEFINED ){
        bool ok = true;
        for(int i=0; i < inputCount() && ok; ++i){
            ok &= input(i).state() != WorkFlowParameter::pkFREE;
        }
        if ( workflow){
            const std::vector<SPWorkFlowNode>& nodes = workflow->nodes();
            int outputsLinked = 0;
            for(auto node : nodes){
                int count = node->type() == WorkFlowNode::ntJUNCTION ? 3 : node->inputCount();
                for(int i=0; i < count; ++i)
                    if ( node->input(i).inputLink()){
                        if (node->input(i).inputLink()->id() == id())
                            outputsLinked++;
                    }
            }
            ok &= outputsLinked == operation()->outputParameterCount();
        }

        return ok;
    }
    return false;
}

QString OperationNode::label() const
{
    QString lbl = WorkFlowNode::label();
    if ( lbl == "")
        return name();
    return lbl;
}
