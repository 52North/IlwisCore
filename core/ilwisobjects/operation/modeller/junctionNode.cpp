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
#include "conditionNode.h"
#include "junctionNode.h"

using namespace Ilwis;

JunctionNode::JunctionNode() :  WorkFlowNode("")
{
    _inputParameters1.resize(3);

}

JunctionNode::JunctionNode(quint64 nodeid) :  WorkFlowNode("")
{
    nodeId(nodeid);
    _inputParameters1.push_back(WorkFlowParameter(WorkFlowCondition::cpTEST,id(),"condition"));
    _inputParameters1.push_back(WorkFlowParameter(WorkFlowCondition::cpTRUECASE,id(),"trueLink"));
    _inputParameters1.push_back(WorkFlowParameter(WorkFlowCondition::cpFALSECASE,id(),"falseLink"));
}

WorkFlowNode::NodeTypes JunctionNode::type() const
{
    return WorkFlowNode::ntJUNCTION   ;
}

void JunctionNode::link2condition(SPWorkFlowNode conditionnode)
{
    WorkFlowParameter parm(WorkFlowCondition::cpTEST,id(),sUNDEF);
    parm.inputLink(conditionnode, iUNDEF);
    _inputParameters1[WorkFlowCondition::cpTEST] = parm;
    if ( conditionnode)
        name(QString("junction_%1_%2").arg(conditionnode->id()).arg(id()));
    else
        name(QString("junction_%1").arg(id()));
}

void JunctionNode::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("junction_%1").arg(id));
}

bool JunctionNode::isValid(const Workflow *workflow, WorkFlowNode::ValidityCheck) const
{
    WorkFlowParameter trueParm = input(WorkFlowCondition::cpTRUECASE);
    WorkFlowParameter falseParm = input(WorkFlowCondition::cpFALSECASE);

    bool ok = trueParm.inputLink() && falseParm.inputLink();
    if ( ok){
        ok = trueParm.valueType() == falseParm.valueType();
        if ( ok){
            // we now try to find the junction as input in one of the nodes of the graph, else the junction is not correctly linked and thus invalid(incomplete)
           const std::vector<SPWorkFlowNode>& nodes = workflow->graph();
           for(auto node : nodes){
               for(int i=0; i < node->inputCount(); ++i)
               if ( node->input(i).inputLink()){
                   if (node->input(i).inputLink()->id() == id())
                       return true;
               }
           }
           return false;
        }
    }
    return ok;

}

void JunctionNode::link2trueCase(SPWorkFlowNode trueNode, int parmIndex)
{
    WorkFlowParameter parm(WorkFlowCondition::cpTRUECASE,id(),sUNDEF);
    parm.inputLink(trueNode, parmIndex);
    _inputParameters1[WorkFlowCondition::cpTRUECASE] = parm;
}

void JunctionNode::link2falseCase(SPWorkFlowNode falseNode, int parmIndex)
{
    WorkFlowParameter parm(WorkFlowCondition::cpFALSECASE,id(),sUNDEF);
    parm.inputLink(falseNode, parmIndex);
    _inputParameters1[WorkFlowCondition::cpFALSECASE] = parm;
}

int JunctionNode::inputCount() const
{
    return 1; // though there are three parameters in the junction, there is only one that is not optional and that is the link to the condition (actually thet tests)
}
