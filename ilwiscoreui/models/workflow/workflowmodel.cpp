#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "rangenode.h"
#include "rangejunctionnode.h"
#include "junctionNode.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "commandhandler.h"
#include "featurecoverage.h"
#include "operationcatalogmodel.h"
#include "../workflowerrormodel.h"
#include "ilwiscontext.h"
#include "oshelper.h"
#include "operationhelper.h"
#include "ilwistypes.h"

using namespace Ilwis;
using namespace boost;

quint32 WorkflowModel::_baserunid = 0;

WorkflowModel::WorkflowModel()
{
}

WorkflowModel::~WorkflowModel()
{
}

WorkflowModel::WorkflowModel(const Ilwis::Resource &source, QObject *parent) : OperationModel(source, parent)
{
    _workflow.prepare(source);
    connect(this, &WorkflowModel::sendMessage, kernel(), &Kernel::acceptMessage);
    connect(kernel(), &Kernel::sendMessage, this, &WorkflowModel::acceptMessage);
    _runid = _baserunid++;
}



quint32 WorkflowModel::addNode(const QString &id, const QVariantMap& parameters)
{
    if ( _workflow.isValid()){
        bool ok;
        quint64 objid = id.toULongLong(&ok);
        if ( ok){
            WorkFlowNode *nodePtr = 0;
            WorkFlowNode::NodeTypes nodeType = string2nodetype(parameters["type"].toString());
            if ( nodeType == WorkFlowNode::ntCONDITION)
                nodePtr = new WorkFlowCondition();
            else if ( nodeType == WorkFlowNode::ntJUNCTION)
                nodePtr = new JunctionNode();
            else if ( nodeType == WorkFlowNode::ntRANGE)
                nodePtr = new RangeNode();
            else if ( nodeType == WorkFlowNode::ntRANGEJUNCTION)
                nodePtr = new RangeJunctionNode();
            else if ( nodeType== WorkFlowNode::ntOPERATION)
                nodePtr = new OperationNode(objid);
            if ( nodePtr){
                SPWorkFlowNode node;
                node.reset(nodePtr);
                quint64 ownerid = i64UNDEF;
                if ( parameters.contains("owner") ){
                    ownerid = parameters["owner"].toInt();
                    node->owner(_workflow->nodeById((ownerid)));
                }

                Pixel lu(parameters["x"].toInt(), parameters["y"].toInt());
                Pixel rd(lu.x + parameters["w"].toInt(), lu.y + parameters["w"].toInt());
                node->box(BoundingBox(lu, rd));
                auto id =  _workflow->addNode(node, ownerid);
                emit changedChanged();
                return id;
            }else {
                kernel()->issues()->log(TR("Could not create node in the workflow; maybe illegal node type?"));
            }
        }
    }
    emit validChanged();
    return iUNDEF;
}



void WorkflowModel::addFlow(int nodeIdFrom, int nodeIdTo, qint32 inParmIndex, qint32 outParmIndex, int rectFrom, int rectTo)
{
    try {
        if ( _workflow.isValid()){
            _workflow->addFlow(nodeIdFrom, nodeIdTo, inParmIndex, outParmIndex, rectFrom, rectTo);
            emit validChanged();
        }
    }
    catch(const ErrorObject&){}
}

void WorkflowModel::addJunctionFlows(int junctionIdTo, const QString &operationIdFrom, int parameterIndex, int rectFrom, int rectTo, bool truecase)
{
    try {
        if ( _workflow.isValid()){

            _workflow->addJunctionFlow(junctionIdTo, operationIdFrom, parameterIndex, rectFrom, rectTo, truecase);
               emit validChanged();
        }
    } catch ( const ErrorObject& ){}
}

void WorkflowModel::addConditionFlow(int operationIdFrom, const QString &conditionIdTo, int testIndex, int inParameterIndex, int outParmIndex, int rectFrom, int rectTo)
{
    try {
        if ( _workflow.isValid()){

            _workflow->addConditionFlow(operationIdFrom, conditionIdTo.toULongLong(), testIndex, inParameterIndex, outParmIndex, rectFrom, rectTo);
               emit validChanged();
        }
    }
    catch(const ErrorObject&){}
}

void WorkflowModel::addTest2Condition(int conditionId, const QString &operationId, const QString &pre, const QString &post)
{
    try {
        SPWorkFlowNode node = _workflow->nodeById(conditionId);
        if ( node){
            std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
            SPWorkFlowNode operNode(new OperationNode(operationId.toULongLong()));
            if ( operNode->id() == i64UNDEF)
                operNode->nodeId(_workflow->generateId());
            LogicalOperator lpre = MathHelper::string2logicalOperator(pre);
            LogicalOperator lpost = MathHelper::string2logicalOperator(post);

            condition->addTest(operNode, lpre, lpost);
            operNode->conditionIdOfTest(condition->id());
               emit validChanged();
        }
    } catch(const ErrorObject&){}
}

void WorkflowModel::addCondition2Junction(int conditionId,int junctionId)
{
    try{
        SPWorkFlowNode node = _workflow->nodeById(conditionId);
        if ( node && node->type() == WorkFlowNode::ntCONDITION){
            std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
            SPWorkFlowNode jnode = _workflow->nodeById(junctionId);
            if ( jnode){
                std::shared_ptr<JunctionNode> junction = std::static_pointer_cast<JunctionNode>(jnode);
                junction->link2condition(node);
                   emit validChanged();
            }

        }
        if ( node && node->type() == WorkFlowNode::ntRANGE){
            std::shared_ptr<RangeNode> condition = std::static_pointer_cast<RangeNode>(node);
            SPWorkFlowNode jnode = _workflow->nodeById(junctionId);
            if ( jnode){
                std::shared_ptr<RangeJunctionNode> junction = std::static_pointer_cast<RangeJunctionNode>(jnode);
                emit validChanged();
            }

        }
    } catch(const ErrorObject&){}
}

void WorkflowModel::setTestValues(int conditionId, int testIndex, int parameterIndex, const QString &value)
{
    try{
        if ( _workflow.isValid()){
            SPWorkFlowNode node = _workflow->nodeById(conditionId);
            if ( node){
                std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
                condition->setTestValue(testIndex, parameterIndex, value, _workflow);
                   emit validChanged();
            }
        }
    } catch(const ErrorObject&){}
}

void WorkflowModel::setRangeTestValues(int rangeid, const QString &rangeDef)
{
    try {
        if ( _workflow.isValid()){
            SPWorkFlowNode node = _workflow->nodeById(rangeid);
            if ( node){
                std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
                range->setRangeDefinition(rangeDef.trimmed(), _workflow.ptr());
            }
        }
    } catch(const ErrorObject&){}
}

QString WorkflowModel::testValueDataType(quint32 conditionId, quint32 testIndex, quint32 parameterIndex) const
{
    if ( _workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(conditionId);
        if ( node){
            std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
            WorkFlowCondition::Test test = condition->test(testIndex);
            if ( test._operation){
                std::shared_ptr<OperationNode> opNode = std::static_pointer_cast<OperationNode>(test._operation);
                SPOperationParameter parm = opNode->operation()->inputParameter(parameterIndex);
                if ( parm){
                    IlwisTypes tp = parm->type();
                    return TypeHelper::type2name(tp);
                }
            }
        }
    }
    return sUNDEF;
}

QString WorkflowModel::testValue(int conditionId, int testIndex, int parameterIndex){
    if ( _workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(conditionId);
        if ( node){
            std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
            return condition->testValue(testIndex, parameterIndex, _workflow);
        }
    }
    return sUNDEF;
}

void WorkflowModel::changeBox(int nodeId, int x, int y, int w, int h)
{
    if ( _workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(nodeId);
        if ( node){
            node->box(BoundingBox(Pixel(x,y), Pixel(x+w, y + h)));
        }
    }
}

bool WorkflowModel::hasValueDefined(int nodeId, int parameterIndex){
      if ( _workflow.isValid()){
          return _workflow->isParameterValueDefined(nodeId, parameterIndex);
      }
      return false;
}

/**
 * Returns the number of input parameters of an operations, this is the same amount as can be seen in the run/workflow form.
 * @param operationIndex the operation who's input parameter count to get.
 * @return The number of input parameters
 */
int WorkflowModel::operationInputParameterCount(int nodeId){
    if ( _workflow.isValid()){
        return _workflow->operationInputParameterCount(nodeId);
    }
    return iUNDEF;
}

/**
 * Returns the number of output parameters of an operations, this is the same amount as can be seen in the run/workflow form.
 * @param operationIndex the operation who's output parameter count to get.
 * @return The number of output parameters
 */
int WorkflowModel::operationOutputParameterCount(int nodeId){
    if ( _workflow.isValid()){
        return _workflow->operationOutputParameterCount(nodeId);
    }
    return iUNDEF;
}

int WorkflowModel::freeInputParameterCount(int nodeId)
{
    int count = 0;
    if ( _workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(nodeId);
        if ( node && node->type()== WorkFlowNode::ntOPERATION){
            for(int i=0; i < node->inputCount(); ++i){
                if ( node->inputRef(i).state() == WorkFlowParameter::pkFREE)
                    ++count;
            }
        }
    }
    return count;
}

bool checkJunctionParameters(SPWorkFlowNode source,SPWorkFlowNode target, int sourceParmIndex, int targetParmIndex, bool trueCase){
    bool checkParameters = sourceParmIndex >= 0 && targetParmIndex >= 0;
    bool ok =  !target->inputRef(trueCase ? 1 : 2).inputLink();
    // if no checkparameter is need we can leave here
    if (!checkParameters || !ok){
        if (!ok)
            kernel()->issues()->log(TR("Tried to set a link that is already in use"), IssueObject::itWarning);
        return ok;
    }
    // we can only check parameters if the false link of the junction points to something
    if (!target->inputRef(trueCase ? 2 : 1).inputLink())
        return ok;
    if ( targetParmIndex >= target->inputCount() || sourceParmIndex >= source->inputCount())
        return false;
    IlwisTypes tpFalse = target->inputRef(2).inputLink()->inputRef(targetParmIndex).valueType();
    IlwisTypes tpTrue = source->inputRef(sourceParmIndex).valueType();
    ok =  hasType(tpFalse, tpTrue);
    if (!ok)
        kernel()->issues()->log(TR("Value types of the true and false case (links) are not compatible"), IssueObject::itWarning);
    return ok;
}

bool WorkflowModel::usableLink(int sourceNodeId, int targetNodeId, int sourceParmIndex, int targetParmIndex)
{
    if ( sourceNodeId == targetNodeId)
        return false;
    SPWorkFlowNode source = _workflow->nodeById(sourceNodeId);
    SPWorkFlowNode target = _workflow->nodeById(targetNodeId);
    if (!source || !target)
        return false;

    // from operation in a condition to a junction
    if ( source->owner() && target->type() == WorkFlowNode::ntJUNCTION){
        //this is a "true" case going to the junction. valid if parameter 1 is empty (not used)
        return checkJunctionParameters(source, target,sourceParmIndex, targetParmIndex, true);
    }else if ( source->type()== WorkFlowNode::ntOPERATION && target->type() == WorkFlowNode::ntJUNCTION){ // false case of junction
        return checkJunctionParameters(source, target,sourceParmIndex, targetParmIndex, false);
    }else if ( source->type()== WorkFlowNode::ntOPERATION && target->type()== WorkFlowNode::ntOPERATION){
        if (sourceParmIndex >= 0 && targetParmIndex >= 0 ){
            IlwisTypes tpTarget = target->inputRef(targetParmIndex).valueType();
            SPOperationParameter outs = source->operation()->outputParameter(sourceParmIndex);
            if ( outs){
                IlwisTypes tpSource = outs->type();
                bool ok = hasType(tpTarget, tpSource);
                if (!ok){
                    kernel()->issues()->log(TR("Value types of input and output of connected operations is not compatible"), IssueObject::itWarning);
                }
                return ok;
            }
        }
    }
    return false;
}


void WorkflowModel::removeNode(int nodeId)
{
    try{
        if ( _workflow.isValid()){
            _workflow->removeNode(nodeId);
               emit validChanged();
        }
    } catch(ErrorObject& err){}
}

void WorkflowModel::deleteFlow(int nodeId, int parameterIndex)
{
    try {
        if ( _workflow.isValid()){
            _workflow->removeFlow(nodeId, parameterIndex);
               emit validChanged();
        }
    } catch(ErrorObject& err){}
}

QVariantMap WorkflowModel::getParameter(const SPWorkFlowNode& node, int index)
{
    QVariantMap parm;
    WorkFlowParameter& p = node->inputRef(index);
    parm["name"] = p.name();
    parm["label"] = p.label();
    parm["description"] = p.description();
    parm["index"] = index;
    parm["flowlabel"] = p.flowLabel();
    parm["outputIndex"] = p.outputParameterIndex() == iUNDEF ? -1 : p.outputParameterIndex();
    if ( p.outputParameterIndex() != iUNDEF)
        parm["outputNodeId"] = p.inputLink()->id();
    parm["valuetype"] = TypeHelper::type2name(p.valueType());
    parm["state"] = p.state() == WorkFlowParameter::pkFREE ? "free" : (p.state() == WorkFlowParameter::pkFIXED ? "fixed" : "calculated");
    parm["sourceRect"] = p.attachement(true);
    parm["targetRect"] = p.attachement(false);
    parm["value"] = p.value();

    return parm;
}

QVariantMap WorkflowModel::translation() const
{
    QVariantMap xy;
    if ( _workflow.isValid()){
        std::pair<int,int> tr = _workflow->translation();
        xy["x"] = tr.first;
        xy["y"] = tr.second;
    }else{
        xy["x"] = 0;
        xy["y"] = 0;
    }
    return xy;
}

void WorkflowModel::translateObject(int x,int y,bool relative)
{
    if ( _workflow.isValid()){
        _workflow->translation(x,y, relative);
    }
}

double WorkflowModel::scale() const
{
    if (_workflow.isValid())
        return _workflow->scale();
    return 1.0;
}

void WorkflowModel::scale(double s)
{
    if ( _workflow.isValid()){
        _workflow->scale(s);
    }
}


QString WorkflowModel::nodetype2string(WorkFlowNode::NodeTypes ntype) const{
    switch(ntype){
    case WorkFlowNode::ntOPERATION:
        return "operationnode";
    case WorkFlowNode::ntJUNCTION:
        return "junctionnode";
    case WorkFlowNode::ntCONDITION:
        return "conditionnode";
    case WorkFlowNode::ntRANGE:
        return "rangenode";
    case WorkFlowNode::ntRANGEJUNCTION:
        return "rangejunctionnode";
    default:
        return sUNDEF;
    }
}

WorkFlowNode::NodeTypes WorkflowModel::string2nodetype(const QString &ntype) const
{
    if (ntype == "operationnode"){
        return WorkFlowNode::ntOPERATION;
    }
    if (ntype == "junctionnode"){
        return WorkFlowNode::ntJUNCTION;
    }
    if (ntype == "conditionnode"){
        return WorkFlowNode::ntCONDITION;
    }
    if (ntype == "rangenode"){
        return WorkFlowNode::ntRANGE;
    }
    if (ntype == "rangejunctionnode"){
        return WorkFlowNode::ntRANGEJUNCTION;
    }
    return WorkFlowNode::ntUNDEFINED;
}

void WorkflowModel::stepMode(bool yesno)
{

    _stepMode = yesno;
    toggleStepMode();
}

bool WorkflowModel::stepMode() const
{
    return _stepMode;
}

qint32 WorkflowModel::runid() const
{
    return _runid;
}

//for efficiency reasons; info is also contained in getNode but this function is called manytimes in bigger flows
QString WorkflowModel::getFlowLabel(int nodeId, int parameterIndex) const{
    SPWorkFlowNode node = _workflow->nodeById(nodeId)    ;
    if (node){
        if ( parameterIndex <  node->inputCount() ){
            WorkFlowParameter& p = node->inputRef(parameterIndex);
            return p.flowLabel();
        }
    }
    return "";
}

QString WorkflowModel::getNodeType(int nodeId) const
{
    SPWorkFlowNode node = _workflow->nodeById(nodeId)    ;
    if (!node)
        return sUNDEF;
    return nodetype2string(node->type());
}

QVariantMap WorkflowModel::getNode(int nodeId){
    QVariantMap data;
    try {
    if (_workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(nodeId)    ;
        if (!node)
            return data;
        IOperationMetaData op = node->operation();
        quint64 oid = op.isValid() ? op->id() : i64UNDEF;
        quint64 nid = node->id();
        BoundingBox box = node->box();
        data["name"] = node->name();
        data["label"] = node->label();
        data["description"]= node->description();
        data["operationid"] = QString::number(oid);
        data["nodeid"] = QString::number(nid);
        data["type"] = nodetype2string(node->type());
        data["x"] = box.min_corner().x;
        data["y"] = box.min_corner().y;
        data["w"] = box.xlength();
        data["h"] = box.ylength();
        if ( node->type()== WorkFlowNode::ntOPERATION){
            QVariantList parameters;
            for(int i=0; i < node->inputCount(); ++i){
                parameters.push_back(getParameter(node, i));
            }
            data["parameters"] = parameters;
        }else if ( hasType(node->type(), WorkFlowNode::ntCONDITION)){
            QVariantList test, operations, junctions;
            std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
            for(int i=0; i < condition->testCount(); ++i){
                WorkFlowCondition::Test tst = condition->test(i);
                QVariantMap t;
                t["pre"] = MathHelper::logicalOperator2string(tst._pre);
                t["post"] = MathHelper::logicalOperator2string(tst._post);
                t["operation"] = getNode(tst._operation->id());
                test.push_back(t);
            }
            std::vector<SPWorkFlowNode> subnodes = node->subnodes("operations");
            for(int i=0; i < subnodes.size(); ++i){
                operations.push_back(getNode(subnodes[i]->id()));
            }
            data["ownedoperations"] = operations;
            subnodes = node->subnodes("junctions");
            for(int i=0; i < subnodes.size(); ++i){
                junctions.push_back(getNode(subnodes[i]->id()));
            }
            data["ownedjunctions"] = junctions;
            data["tests"] = test;
        }if ( hasType(node->type(), WorkFlowNode::ntRANGE)){
              QVariantList operations, junctions, parameters;

              std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
              QString def =  range->rangeDefinition();
              data["rangedefinition"] = def;
              parameters.push_back(getParameter(node, 0));
              data["parameters"] = parameters;
              std::vector<SPWorkFlowNode> subnodes = node->subnodes("operations");
              for(int i=0; i < subnodes.size(); ++i){
                  operations.push_back(getNode(subnodes[i]->id()));
              }
              subnodes = node->subnodes("junctions");
              for(int i=0; i < subnodes.size(); ++i){
                  junctions.push_back(getNode(subnodes[i]->id()));
              }
              data["ownedoperations"] = operations;
              data["ownedjunctions"] = junctions;
        }else if ( node->type() == WorkFlowNode::ntRANGEJUNCTION){
            QVariantList parameters;
            std::shared_ptr<RangeJunctionNode> junction = std::static_pointer_cast<RangeJunctionNode>(node);
            for(int i=0; i <4; ++i){
                parameters.push_back(getParameter(junction, i));
            }
            data["parameters"] = parameters;

        }else if ( node->type() == WorkFlowNode::ntJUNCTION){
            std::shared_ptr<JunctionNode> junction = std::static_pointer_cast<JunctionNode>(node);
            SPWorkFlowNode testNode =  junction->inputRef(WorkFlowCondition::cpTEST).inputLink();
            data["linkedcondition"] = testNode ? testNode->id() : i64UNDEF;
            data["linkedtrueoperation"] = getParameter(junction,WorkFlowCondition::cpTRUECASE);
            data["linkedfalseoperation"] = getParameter(junction,WorkFlowCondition::cpFALSECASE);
        }
    }
    } catch(ErrorObject&){}
    return data;
}

QVariantList WorkflowModel::getTestParameters(int nodeId, int testIndex) {
    QVariantList parameters;
    QVariantList all = getTests(nodeId);
    if ( all.size() > 0 && testIndex < all.size()){
        parameters = all[testIndex].toMap()["parameters"].toList();
    }
    return parameters;
}


void WorkflowModel::updateOperationParameters(quint32 nodeid, int order, const QString &txt)
{
    if (_workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(nodeid);
        if ( node && node->type() == WorkFlowNode::ntOPERATION){
            if ( order < node->inputCount())
                node->inputRef(order).label(txt);
        }
    }
}

QVariantList WorkflowModel::getTests(int conditionId) const
{
    try {
    QVariantList result;
    if (_workflow.isValid()){
        SPWorkFlowNode node = _workflow->nodeById(conditionId)    ;
        if (!node || !hasType(node->type(),WorkFlowNode::ntCONDITION |WorkFlowNode::ntRANGE))
            return result;

        std::shared_ptr<WorkFlowCondition> condition = std::static_pointer_cast<WorkFlowCondition>(node);
        for(int i=0; i < condition->testCount(); ++i){
            WorkFlowCondition::Test test = condition->test(i);
            std::shared_ptr<OperationNode> operNode = std::static_pointer_cast<OperationNode>(test._operation);
            QVariantMap testEntry;
            testEntry["name2"] =operNode->name();
            testEntry["pre"] = MathHelper::logicalOperator2string(test._pre);
            testEntry["post"] = MathHelper::logicalOperator2string(test._post);
            QVariantList parameters;
            for(int j=0; j < operNode->inputCount(); ++j){
                QVariantMap parameterKeys;
                QString value;
                if ( test._operation->inputRef(j).inputLink()){
                    int n = test._operation->inputRef(j).outputParameterIndex();
                    value = "link=" + QString::number(test._operation->inputRef(j).inputLink()->id()) + ":" + QString::number(n);
                }else
                    value = operNode->inputRef(j).value();
                QString label = operNode->inputRef(j).label();
                testEntry["name2"] = testEntry["name2"].toString() + " " + value;
                parameterKeys["value"] = value;
                parameterKeys["label"] = label;
                QStringList choices = operNode->operation()->choiceList(j);
                QVariantList lst;
                for(QString choice : choices)
                    lst.append(choice);
                parameterKeys["choicelist"] = lst;
                parameters.push_back(parameterKeys);
            }
            testEntry["parameters"] = parameters;
            result.push_back(testEntry);
        }
    }
    return result;
    } catch (const ErrorObject&){
        kernel()->issues()->log(TR("Could not create condition tests"));
    }

    return QVariantList();
}

QVariantList WorkflowModel::getNodes()
{
    QVariantList result;
    try {
        const std::vector<SPWorkFlowNode>& graph = _workflow->graph();

        for(const SPWorkFlowNode& node : graph){
            QVariantMap data = getNode(node->id());

            result.push_back(data);

        }
        return result;
    }
    catch ( const ErrorObject& err){}
    return result;

}

void WorkflowModel::createMetadata()
{
    try {
        _workflow->createMetadata();
    } catch (const ErrorObject& err){

    }
}

QVariantList WorkflowModel::propertyList()
{
    QVariantList result;
    try{


        QVariantMap workflow;
        workflow["label"] = TR("Edit Workflow(Run) form");
        workflow["form"] = "OperationForms.qml";
        result.append(workflow);
        QVariantMap operation;
        operation["label"] = TR("Edit Selected operation form");
        operation["form"] = "OperationForms.qml";
        result.append(operation);
        QVariantMap metadata;
        metadata["label"] = TR("Metadata selected form");
        metadata["form"] = "workflow/forms/OperationPropForm.qml";
        result.append(metadata);
        QVariantMap script;
        metadata["label"] = TR("Script");
        metadata["form"] = "workflow/forms/WorkflowPythonScript.qml";
        result.append(metadata);

        return result;
    } catch(const ErrorObject&){}

    return result;
}

QString WorkflowModel::generateScript(const QString &type, const QString& parameters)
{
    try {
        QString result;
        if ( type == "python"){
            _expression = OperationExpression::createExpression(_workflow->id(),parameters,true);
            if ( !_expression.isValid())
                return "";
            QString expr = _expression.toString();
            int index = expr.indexOf("script ");
            if ( index == 0){
                expr = expr.mid(7);
            }
            index = expr.indexOf(")}=");
            if ( index != -1){
                expr = expr.mid(index + 3);
            }

            OperationExpression expression = OperationExpression(expr);
            QUrl url = _workflow->resource().url(true);
            QFileInfo inf(url.toLocalFile());
           // QUrl newName = QUrl::fromLocalFile(inf.path() + "/" + inf.baseName() + ".py");
            QUrl newName = INTERNAL_CATALOG + "/" + inf.baseName() + ".ilwis";
            _workflow->connectTo(newName,"inmemoryworkflow","python",IlwisObject::cmOUTPUT);
            QVariant value;
            value.setValue(expression);
            IOOptions opt("expression", value);
            _workflow->store(opt);
            result = _workflow->constConnector(IlwisObject::cmOUTPUT)->getProperty("content").toString();
        }
        return result;
    }
    catch (ErrorObject&){
    }
    return "";
}

void WorkflowModel::toggleStepMode()
{
    QVariantMap parms;
    parms["stepmode"] = _stepMode;
    parms["id"] = _workflow->id();
    parms["runid"] = _runid;
    emit sendMessage("workflow","stepmode", parms);

}

void WorkflowModel::resetStepMode()
{
    _outputsCurrentOperation = QVariantMap();
    QVariantList _outputs = QVariantList();
    emit outputCurrentOperationChanged();
}

void WorkflowModel::nextStep()
{
    bool ok;
    QWaitCondition &waitc = kernel()->waitcondition(_runid, ok);
    if ( ok){
        waitc.wakeAll();
    }
}

void WorkflowModel::stopStepMode(){
    QVariantMap parms;
    parms["id"] = _workflow->id();
    parms["runid"] = _runid;
    _stepMode = false;
    _lastOperationNode = -1;
    _currentNode = -1;
    emit sendMessage("workflow","stopstepmode", parms);
    emit operationNodeChanged();
    emit currentNodeChanged();
}

void WorkflowModel::stopExecution()
{
    if ( _stepMode)
        stopStepMode();
    else{
        QVariantMap parms;
        parms["id"] = _workflow->id();
        parms["runid"] = _runid;
        _stepMode = false;
        _lastOperationNode = -1;
        _currentNode = -1;
        emit sendMessage("workflow","stopexecution", parms);
        emit operationNodeChanged();
        emit currentNodeChanged();
    }

}

void WorkflowModel::store(const QString& container, const QString& name)
{
    if ( container == "" && name == ""){
        _workflow->store();
    } else if ( container == "" && name != "") {
        _workflow->name(name);
        _workflow->store();
    }else if ( OSHelper::neutralizeFileName(container) == OSHelper::neutralizeFileName(_workflow->resource().container().toString())){
        if ( name == "")    {
            _workflow->store();
        }else {
            _workflow->name(name);
            _workflow->store();
        }
    }else {
        QString cont = container;
        if ( container.indexOf(".ilwis") != 0){ // oops somebody put a file, not a container
            int index = container.lastIndexOf("/");
            cont = container.left(index);
        }
        QString newName = name == "" ? _workflow->name() : name;
        QUrl newUrl = cont + "/" + newName;
        _workflow->connectTo(newUrl,"workflow","stream",IlwisObject::cmOUTPUT);
        _workflow->store();
    }
}

void WorkflowModel::setFixedValues(qint32 nodeid, const QString &formValues)
{
    try {
        SPWorkFlowNode node = _workflow->nodeById(nodeid);
        if (!node)
            return;

        IOperationMetaData op = node->operation();

        QStringList inputParameters = formValues.split('|');
        for (int i = 0; i < inputParameters.length(); ++i) {
            QString value = inputParameters[i].trimmed();
            WorkFlowParameter& parm = node->inputRef(i);
            parm.value(value, op->getInputParameters()[i]->type(), WorkFlowParameter::pkFIXED);
        }
    }catch(ErrorObject&){}

}

bool WorkflowModel::isValidNode(qint32 nodeid,const QString& part) const
{
    SPWorkFlowNode node = _workflow->nodeById(nodeid);
    if (!node)
        return false;
    WorkFlowNode::ValidityCheck vc = WorkFlowNode::vcPARTIAL;
    if (part != ""){
        if ( hasType(node->type(),WorkFlowNode::ntCONDITION|WorkFlowNode::ntRANGE)){
            if ( part == "tests")
                vc = WorkFlowNode::vcTESTS ;
            if ( part == "operations")
                vc = WorkFlowNode::vcOPERATIONS;
            if ( part == "junctions")
                vc = WorkFlowNode::vcJUNCTIONS;
        }
    }
    if ( node->owner() && node->owner()->type() == WorkFlowNode::ntCONDITION){
        vc = WorkFlowNode::vcALLDEFINED;
    }
    return node->isValid(_workflow.ptr(), vc);
}

QQmlListProperty<IlwisObjectModel> WorkflowModel::selectedOperation()
{
    return  QQmlListProperty<IlwisObjectModel>(this, _selectedOperation);
}

void WorkflowModel::selectOperation(const QString& id)
{
    IOperationMetaData op;
    op.prepare(id.toULongLong());
    if ( op.isValid()){
        _selectedOperation.clear();
        _selectedOperation.append(new IlwisObjectModel(op->resource(),this));
    }
    emit selectionChanged();

}

bool WorkflowModel::collapsed(int nodeid) const
{
   if ( _workflow.isValid())    {
       SPWorkFlowNode node =  _workflow->nodeById(nodeid);
       if ( node)
           return node->collapsed();
   }
   return false;
}

void WorkflowModel::collapsed(int nodeid, bool yesno)
{
    if ( _workflow.isValid())    {
        SPWorkFlowNode node =  _workflow->nodeById(nodeid);
        if ( node)
            node->collapsed(yesno);
    }
}

void WorkflowModel::setNodeProperty(int nodeid, int paramindex, const QString &property, const QString& value)
{
    if ( _workflow.isValid())    {
        SPWorkFlowNode node =  _workflow->nodeById(nodeid);
        if ( node){
            if ( property == "description"){
                if (   paramindex < 0){
                    node->setDescription(value);
                }else {
                    if ( paramindex < node->inputCount()){
                        node->inputRef(paramindex).setDescription(value);
                    }
                }
            }else if ( property == "label"){
                if (   paramindex < 0){
                    node->label(value);
                }else {
                    if ( paramindex < node->inputCount()){
                        node->inputRef(paramindex).label(value);
                    }
                }
            }else if ( property == "flowlabel"){
                if ( paramindex < node->inputCount() && paramindex >=0){
                    node->inputRef(paramindex).flowLabel(value);
                }
            }
        }
    }
}


bool WorkflowModel::isValid() const
{
    if ( _workflow.isValid())    {
        return _workflow->isValid();
    }
    return false;
}

void WorkflowModel::acceptMessage(const QString &type, const QString &subtype, const QVariantMap &parameters)
{
    if ( type == "workflow"){
        bool ok;
        quint64 id = parameters["runid"].toLongLong(&ok);
        if ( ok && id == _runid){ // check if this was meant for this workflow
            if ( subtype == "outputdata"){
                _outputsCurrentOperation = parameters;
                QVariantList newlist = _outputsCurrentOperation["results"].value<QVariantList>();
                _lastOperationNode = newlist[0].toMap()["node"].toInt();

                _outputs.append(newlist);
                emit outputCurrentOperationChanged();
                emit operationNodeChanged();
            }else if ( subtype == "lastrunnode"){
                bool ok;
                quint64 conditionid = parameters["condtionid"].toULongLong(&ok);
                if ( ok && conditionid != i64UNDEF){
                    _lastOperationNode = conditionid;
                }else {
                    _lastOperationNode = parameters["node"].toInt();
                }

            }else if ( subtype == "currentnode"){
                quint64 conditionid = parameters["condtionid"].toULongLong(&ok);
                if ( ok && conditionid != i64UNDEF){
                    _currentNode = conditionid;
                }else {
                    _currentNode = parameters["node"].toInt();
                }
                emit currentNodeChanged();
            }else if ( subtype == "stopexecution"){
                _stepMode = false;
                _lastOperationNode = -1;
                _currentNode = -1;
                emit operationNodeChanged();
                emit currentNodeChanged();

            }
        }
    }
    if ( type == "operation"){
        bool ok;
        quint64 id = parameters["runid"].toLongLong(&ok);
        if ( ok && id == _runid){
            if ( subtype == "stopped"){
                _stepMode = false;
                _lastOperationNode = -1;
                _currentNode = -1;
                emit operationNodeChanged();
                emit currentNodeChanged();
            }
        }
    }
}

int WorkflowModel::lastOperationNode() const
{
    return _lastOperationNode;
}

int WorkflowModel::currentNode() const
{
    return _currentNode;
}

QVariantList WorkflowModel::outputCurrentOperation()
{
    return _outputs;
}

 QString WorkflowModel::modelType() const
 {
     return "workflowmodel";
 }

