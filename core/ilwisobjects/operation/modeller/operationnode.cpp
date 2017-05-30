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
         wfp.needsQuotes(parms[i]->needsQuotes());
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



IOperationMetaData OperationNode::operation() const
{
    if ( !_operation.isValid() && _syntax != "" ){
        std::vector<Resource> items = mastercatalog()->select("catalogitemproperties.propertyname='syntax' and catalogitemproperties.propertyvalue='" + _syntax + "'");
        if ( items.size() >= 1){
            const_cast<OperationNode *>(this)->_operation.prepare(items[0]);
        }
    }
    return _operation;
}

void OperationNode::operation(const QString& provider, const QString& syntax, bool isWorkflow)
{
    _provider = provider;
    _syntax = syntax;
    _isWorkflow = isWorkflow;
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

bool OperationNode::isWorkflow() const
{
    return _isWorkflow;
}
