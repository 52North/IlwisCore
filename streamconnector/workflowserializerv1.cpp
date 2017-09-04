#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "operationmetadataserializerv1.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "junctionNode.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "rangenode.h"
#include "rangejunctionnode.h"
#include "workflowserializerv1.h"
#include "workflow.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *WorkflowSerializerV1::create(QDataStream& stream)
{
    return new WorkflowSerializerV1(stream);
}

WorkflowSerializerV1::WorkflowSerializerV1(QDataStream &stream) : OperationMetadataSerializerV1(stream)
{
}

void WorkflowSerializerV1::storeNodeLinks(const SPWorkFlowNode& node) {
    if ( node->type() == WorkFlowNode::ntCONDITION){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 sz = condition->subnodes("tests").size();
        _stream << sz;
        for(qint32 t=0; t < sz; ++t){
            WorkFlowCondition::Test test = condition->test(t)    ;
           storeNodeLinks(test._operation);
        }
        auto operations = condition->subnodes("operations")    ;
        for(qint32 o=0; o < operations.size(); ++o){
            storeNodeLinks(operations[o]);
        }
    }
    if ( node->type() == WorkFlowNode::ntRANGE){
        auto operations = node->subnodes("operations")    ;
        for(qint32 o=0; o < operations.size(); ++o){
            storeNodeLinks(operations[o]);
        }
        auto junctions = node->subnodes("junctions")    ;
        for(qint32 o=0; o < junctions.size(); ++o){
            storeNodeLinks(junctions[o]);
        }
    }
    int count = node->inputCount();
    // junctions have 3 or 4 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
    //links to operations are implicit. Needed for saving though so we overrule here
    if ( node->type() == WorkFlowNode::ntJUNCTION)
        count = 3;
    if ( node->type() == WorkFlowNode::ntRANGEJUNCTION)
        count = 4;

    _stream << count;

    for(qint32 i = 0; i < count; ++i){
        WorkFlowParameter& wp = node->inputRef(i);
        _stream << wp.outputParameterIndex();
        if (wp.isValid()){
            if ( wp.inputLink())
                _stream << wp.inputLink()->id();
            else
                _stream << i64UNDEF;
        }else
            _stream << i64UNDEF;
    }
}

bool WorkflowSerializerV1::storeNode(const SPWorkFlowNode& node, const IOOptions &options){
    _stream << node->name();
    _stream << node->label();
    _stream << node->description();
    _stream << node->id();
    IOperationMetaData op = node->operation();
    if ( op.isValid()){
        QString syntax = op->resource()["syntax"].toString();
        QString provider = op->resource()["namespace"].toString();
        _stream << (bool)(op->ilwisType() == itWORKFLOW);
        _stream << syntax;
        _stream << provider;
    }else{
        _stream << false;
        _stream << sUNDEF;
        _stream << sUNDEF;
    }
    _stream << (qint32)node->type();
    _stream << (qint32)node->collapsed();
    if ( node->type() == WorkFlowNode::ntCONDITION){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 sz = condition->subnodes("tests").size();
        _stream << sz;
        for(qint32 t=0; t < sz; ++t){
            WorkFlowCondition::Test test = condition->test(t)    ;
            _stream << (qint32)test._pre;
            _stream << (qint32)test._post;
           storeNode(test._operation);
        }
        auto operations = condition->subnodes("operations")    ;
        _stream << (int)operations.size();
        for(qint32 o=0; o < operations.size(); ++o){
            storeNode(operations[o]);
        }
    }
    if ( node->type() == WorkFlowNode::ntRANGE){
        std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
        QString rangedef =  range->rangeDefinition( );
        _stream << rangedef;
        qint32 sz = node->subnodes("operations").size();
        _stream << sz;
        auto operations = node->subnodes("operations")    ;
        for(qint32 o=0; o < operations.size(); ++o){
            storeNode(operations[o]);
        }
        sz = node->subnodes("junctions").size();
        _stream << sz;
        auto junctions = node->subnodes("junctions")    ;
        for(qint32 o=0; o < junctions.size(); ++o){
            storeNode(junctions[o]);
        }
    }
    node->box().store(_stream);

    qint32 count = node->inputCount();
    // junctions have 3 or 4 parameters; but inputcount returns only 1 as the link to the condition is the only explicit parameter;
    //links to operations are implicit. Needed for saving though so we overrule here
    if ( node->type() == WorkFlowNode::ntJUNCTION)
        count = 3;
    if ( node->type() == WorkFlowNode::ntRANGEJUNCTION)
        count = 4;

    _stream << count;

    for(qint32 i = 0; i < count; ++i){
        WorkFlowParameter& wp = node->inputRef(i);
        _stream << wp.order();
        _stream << wp.name();
        _stream << wp.description();
        _stream << wp.label();
        _stream << wp.flowLabel();
        _stream << wp.valueType();
        _stream << (qint32)wp.state();
        _stream << wp.attachement(true);
        _stream << wp.attachement(false);
        _stream << wp.syntax();
        _stream << wp.needsQuotes();
        auto line = wp.line();
        _stream << line.size();
        for(int p=0; p< line.size();++p ){
            line[i].store(_stream);
        }
        if(!VersionedSerializer::store(wp.value(), wp.valueType(), options))
            return false;
    }
    return true;
}

bool WorkflowSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::store(obj, options))
        return false;


    _stream << workflow->scale();
    _stream << workflow->translation().first;
    _stream << workflow->translation().second;

    const std::vector<SPWorkFlowNode>& graph = workflow->graph();

    _stream << (int)graph.size();
    for(const SPWorkFlowNode node : graph){
        storeNode(node, options);
    }
    for(const SPWorkFlowNode& node : graph){
        storeNodeLinks(node);
    }
    _dataLoaded = true;
    workflow->changed(false);
    return true;
}

void WorkflowSerializerV1::loadNodeLinks(SPWorkFlowNode& node,Workflow *workflow){

     if ( node->type() == WorkFlowNode::ntCONDITION){
        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        qint32 ocount;
        _stream >> ocount;
        for(qint32 t=0; t < ocount; ++t){
            WorkFlowCondition::Test test = condition->test(t);
            SPWorkFlowNode operationNode;
            loadNodeLinks(test._operation, workflow);
        }
        auto subnodes = condition->subnodes("operations");
        for(SPWorkFlowNode& operationNode : subnodes){
            loadNodeLinks(operationNode, workflow);
        }
    }
     if ( node->type() == WorkFlowNode::ntRANGE){
       // loadNodeLinks(node, workflow);

        auto subnodes = node->subnodes("operations");
        for(SPWorkFlowNode& operationNode : subnodes){
            loadNodeLinks(operationNode, workflow);
        }
        auto junctions = node->subnodes("junctions");
        for(SPWorkFlowNode& junctionNode : junctions){
            loadNodeLinks(junctionNode, workflow);
        }
    }
    qint32 parmCount;
    _stream >> parmCount;

    for(qint32 j=0; j < parmCount; ++j){
        qint32 outParmindex;
        NodeId nodeid;
        _stream >> outParmindex;
        _stream >> nodeid;
        SPWorkFlowNode prevNode;
        if ( nodeid != i64UNDEF)
            prevNode = workflow->nodeById(nodeid);
        if ( prevNode)
            node->inputRef(j).inputLink(prevNode, outParmindex);
    }
}

void WorkflowSerializerV1::loadNode(SPWorkFlowNode& node,Workflow *workflow, const IOOptions &options){

    QString nm, lbl;
    qint32 collapsed, type;
    _stream >> nm;
    _stream >> lbl;
    QString ds;
    _stream >> ds;
    quint64 nodeid;
    _stream >> nodeid;
    QString syntax, provider;
    bool isWorkflow;
    _stream >> isWorkflow;
    _stream >> syntax;
    _stream >> provider;
    _stream >> type;
    _stream >> collapsed;
    if ( type == (qint32)WorkFlowNode::ntRANGEJUNCTION){
        auto opNode = new RangeJunctionNode(nodeid);
        opNode->name(nm);
        opNode->setDescription(ds);
        node.reset(opNode);
    }else  if ( type == (qint32)WorkFlowNode::ntOPERATION){
        auto opNode = new OperationNode(nm, ds,nodeid);
        opNode->operation(provider, syntax, isWorkflow);
        node.reset(opNode);
    }else if ( type == (qint32)WorkFlowNode::ntCONDITION){
        auto cnode = new WorkFlowCondition();
        cnode->nodeId(nodeid);
        node.reset(cnode);
        qint32 pre,post;
        qint32 ocount;
        _stream >> ocount;
        for(qint32 t=0; t < ocount; ++t){
            _stream >> pre;
            _stream >> post;
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
            cnode->addTest(operationNode, (LogicalOperator) pre,(LogicalOperator) post);
            operationNode->conditionIdOfTest(cnode->id());
        }
        _stream >> ocount;
        for(qint32 o=0; o < ocount; ++o){
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
            cnode->addSubNode(operationNode,"operations");
            operationNode->owner(node);
        }

    }else if ( type == (qint32)WorkFlowNode::ntRANGE){
        auto rnode = new RangeNode();
        rnode->nodeId(nodeid);
        node.reset(rnode);
        QString rangedef;
        _stream >> rangedef;
        rnode->setRangeDefinition( rangedef, workflow);
        qint32 ocount;
        _stream >> ocount;
        for(qint32 o=0; o < ocount; ++o){
            SPWorkFlowNode operationNode;
            loadNode(operationNode, workflow);
            rnode->addSubNode(operationNode,"operations");
            operationNode->owner(node);
        }
        _stream >> ocount;
        for(qint32 o=0; o < ocount; ++o){
            SPWorkFlowNode junctionNode;
            loadNode(junctionNode, workflow);
            rnode->addSubNode(junctionNode,"junctions");
            junctionNode->owner(node);
        }

    }else if ( type == (qint32)WorkFlowNode::ntJUNCTION){
        auto cnode = new JunctionNode();
        cnode->nodeId(nodeid);
        node.reset(cnode);

    }
    if (!node){
        _stream.device()->close();
        throw ErrorObject(TR("Stored workflow definition is invalid"));
    }
    node->collapsed(collapsed > 0);
    node->label(lbl);
    BoundingBox box;
    box.load(_stream);
    node->box(box);
    qint32 parmCount;
    _stream >> parmCount;

    for(qint32 j=0; j < parmCount; ++j){
        qint32 order;

        _stream >> order;
        _stream >> nm;
        _stream >> ds;

        QString label, flowlabel;
        _stream >> label;
        _stream >> flowlabel;

        IlwisTypes tp;
        _stream >> tp;
        qint32 state;
        _stream >> state;
        WorkFlowParameter wp(j, node->id(), nm,ds);
        wp.flowLabel(flowlabel);
        wp.label(label);

        qint32 rctIndex;
        _stream >> rctIndex;
        wp.attachement(rctIndex, true);
        _stream >> rctIndex;
        wp.attachement(rctIndex, false);
        QString syntax;
        _stream >> syntax;
        wp.addSyntax(syntax);
        bool quotes;
        _stream >> quotes;
        wp.needsQuotes(quotes);
        size_t sz;
        _stream >> sz;
        std::vector<XY> line;
        for(int p=0; p < sz; ++p){
            XY point;
            point.load(_stream);
            line.push_back(point);
        }
        wp.line(line);
        QString v;
        IOOptions opt = options;
        opt.addOption("mustexist", true);
        if(!VersionedSerializer::loadMetaData(opt, tp, v))
            return;
        wp.value(v, tp);
        node->addInput(wp,j);
    }
}

bool WorkflowSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    Workflow *workflow = static_cast<Workflow *>(obj);
    if (!OperationMetadataSerializerV1::loadMetaData(obj, options))
        return false;
    double scale;
    qint32 translationx, translationy;
    _stream >> scale;
    _stream >> translationx;
    _stream >> translationy;
    workflow->scale(scale);
    workflow->translation(translationx, translationy,false);
    qint32 sz;
    _stream >> sz;
    std::vector<SPWorkFlowNode> independenNodes;
    for(qint32 i = 0; i < sz ; ++i){
        SPWorkFlowNode node;
        loadNode(node, workflow);

        workflow->addNode(node);
        independenNodes.push_back(node);
    }
    for(SPWorkFlowNode& node : independenNodes){
        loadNodeLinks(node, workflow);
    }

    workflow->updateIdCounter();

    return true;
}
