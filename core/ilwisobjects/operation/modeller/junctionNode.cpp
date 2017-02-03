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

Junction::Junction() :  WorkFlowNode("")
{
    _inputParameters1.resize(3);

}

Junction::Junction(quint64 nodeid) :  WorkFlowNode("")
{
    nodeId(nodeid);
    _inputParameters1.push_back(WorkFlowParameter(0,id(),"condition"));
    _inputParameters1.push_back(WorkFlowParameter(1,id(),"trueLink"));
    _inputParameters1.push_back(WorkFlowParameter(2,id(),"falseLink"));
}

WorkFlowNode::NodeTypes Junction::type() const
{
    return WorkFlowNode::ntJUNCTION   ;
}

void Junction::link2condition(SPWorkFlowNode conditionnode)
{
    WorkFlowParameter parm(0,id(),sUNDEF);
    parm.inputLink(conditionnode, iUNDEF);
    _inputParameters1[0] = parm;
    if ( conditionnode)
        name(QString("junction_%1_%2").arg(conditionnode->id()).arg(id()));
    else
        name(QString("junction_%1").arg(id()));
}

void Junction::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("junction_%1").arg(id));
}

bool Junction::isValid(const Workflow *workflow, WorkFlowNode::ValidityCheck) const
{
    WorkFlowParameter parm1 = input(1);
    WorkFlowParameter parm2 = input(2);

    bool ok = parm1.inputLink() && parm2.inputLink();
    if ( ok){
        ok = parm1.valueType() == parm2.valueType();
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

void Junction::link2trueCase(SPWorkFlowNode trueNode, int parmIndex)
{
    WorkFlowParameter parm(1,id(),sUNDEF);
    parm.inputLink(trueNode, parmIndex);
    _inputParameters1[1] = parm;
}

void Junction::link2falseCase(SPWorkFlowNode falseNode, int parmIndex)
{
    WorkFlowParameter parm(2,id(),sUNDEF);
    parm.inputLink(falseNode, parmIndex);
    _inputParameters1[2] = parm;
}

int Junction::inputCount() const
{
    return 1; // though there are three parameters in the junction, there is only one that is not optional and that is the link to the condition (actually thet tests)
}
