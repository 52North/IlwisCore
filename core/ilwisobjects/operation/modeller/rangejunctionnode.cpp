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
#include "rangeNode.h"
#include "rangejunctionnode.h"

using namespace Ilwis;

RangeJunctionNode::RangeJunctionNode() : WorkFlowNode("")
{
    _inputParameters1.resize(4);
}

RangeJunctionNode::RangeJunctionNode(quint64 nodeid) : WorkFlowNode("")
{
    nodeId(nodeid);
    _inputParameters1.push_back(WorkFlowParameter(RangeNode::rpOUTPUT,id(),"output"));
    _inputParameters1.push_back(WorkFlowParameter(RangeNode::rpINITIALINPUT,id(),"initialinput"));
    _inputParameters1.push_back(WorkFlowParameter(RangeNode::rpINPUT,id(),"input"));
    _inputParameters1.push_back(WorkFlowParameter(RangeNode::rpFINALOUTPUT,id(),"output"));
}

int RangeJunctionNode::inputCount() const
{
    return 1;
}

void RangeJunctionNode::link2initialInput(SPWorkFlowNode node, int parmIndex)
{
    WorkFlowParameter parm(RangeNode::rpINITIALINPUT,id(),sUNDEF);
    parm.inputLink(node, parmIndex);
    _inputParameters1[RangeNode::rpINITIALINPUT] = parm;
}

void RangeJunctionNode::link2Input(SPWorkFlowNode node, int parmIndex)
{
    WorkFlowParameter parm(RangeNode::rpINPUT,id(),sUNDEF);
    parm.inputLink(node, parmIndex);
    _inputParameters1[RangeNode::rpINPUT] = parm;
}

void RangeJunctionNode::link2Output(Ilwis::SPWorkFlowNode node, int parmIndex)
{
    WorkFlowParameter parm(RangeNode::rpOUTPUT,id(),sUNDEF);
    parm.inputLink(node, parmIndex);
    _inputParameters1[RangeNode::rpOUTPUT] = parm;
}

void RangeJunctionNode::link2FinalOutput(Ilwis::SPWorkFlowNode node, int parmIndex)
{
    WorkFlowParameter parm(RangeNode::rpFINALOUTPUT,id(),sUNDEF);
    parm.inputLink(node, parmIndex);
    _inputParameters1[RangeNode::rpFINALOUTPUT] = parm;
}

void RangeJunctionNode::nodeId(quint64 id)
{
    WorkFlowNode::nodeId(id);
    name(QString("rangejunction_%1").arg(id));
}

void RangeJunctionNode::setFlow(const SPWorkFlowNode &fromNode, qint32 inParmIndex, qint32 outParmIndex, int attachRctIndxFrom, int attachRctIndxTo)
{
    if (!fromNode->owner() || (owner()->id() != fromNode->owner()->id())){ // either it comes from the no container or it jumps from container to other container
        inputRef(RangeNode::rpINITIALINPUT).inputLink(fromNode,outParmIndex);
        inputRef(RangeNode::rpINITIALINPUT).attachement(attachRctIndxFrom, true);
        inputRef(RangeNode::rpINITIALINPUT).attachement(attachRctIndxTo, false);
    }
    else if ( fromNode->owner() && owner()->id() == fromNode->owner()->id() ){ // inside the container
        inputRef(RangeNode::rpINPUT).inputLink(fromNode,outParmIndex);
        inputRef(RangeNode::rpINPUT).attachement(attachRctIndxFrom, true);
        inputRef(RangeNode::rpINPUT).attachement(attachRctIndxTo, false);
    }
}

bool RangeJunctionNode::isValid(const Ilwis::Workflow *workflow, WorkFlowNode::ValidityCheck) const
{
    return true;
}

WorkFlowNode::NodeTypes RangeJunctionNode::type() const
{
    return WorkFlowNode::ntRANGEJUNCTION   ;
}
