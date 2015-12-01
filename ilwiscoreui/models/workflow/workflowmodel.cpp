#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "featurecoverage.h"
#include "workflowerrormodel.h"
#include "../../IlwisCore/core/ilwiscontext.h"

using namespace Ilwis;
using namespace boost;

WorkflowModel::WorkflowModel()
{
}

WorkflowModel::~WorkflowModel()
{
    for(auto *node : _nodeProps)
        delete node;
    for(auto *edge : _edgeProps)
        delete edge;
}

WorkflowModel::WorkflowModel(const Ilwis::Resource &source, QObject *parent) : OperationModel(source, parent)
{
    _workflow.prepare(source);
}

void WorkflowModel::asignConstantInputData(QString inputData, int operationIndex) {
    QStringList inputParameters = inputData.split('|');
    for (int i = 0; i < inputParameters.length(); ++i) {
        QString value = inputParameters[i];
        OVertex vertex = _operationNodes[operationIndex];
        if(_workflow->hasInputAssignment(vertex,i)){
            SPAssignedInputData constantInput = _workflow->getAssignedInputData({vertex, i});
            if (value.trimmed().size() == 0)
                value = value.trimmed();
            constantInput->value = value;
        }
    }
}

void WorkflowModel::addOperation(const QString &id)
{
    bool ok;
    quint64 opid = id.toULongLong(&ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("Invalid operation id used in workflow %1")).arg(name()));
        return ;
    }
    auto vertex = _workflow->addOperation({opid, _workflow->source()});
    _operationNodes.push_back(vertex);

}

void WorkflowModel::addFlow(int operationIndex1, int operationIndex2, const QVariantMap& flowpoints, int outRectIndex, int inRectIndex)
{
    if ( operationIndex1 >= 0 && operationIndex2 >= 0 && flowpoints.size() == 2) {
        try {
            const OVertex& fromOperationVertex = _operationNodes[operationIndex1];
            const OVertex& toOperationVertex = _operationNodes[operationIndex2];
            int outParamIndex = flowpoints["fromParameterIndex"].toInt();
            int inParamIndex = flowpoints["toParameterIndex"].toInt();
            EdgeProperties flowPoperties(
                outParamIndex, inParamIndex,
                outRectIndex, inRectIndex
            );
            _workflow->addOperationFlow(fromOperationVertex,toOperationVertex,flowPoperties);
        } catch (std::out_of_range e) {
           qDebug() << "False operation";
        }
    }
}

bool WorkflowModel::hasValueDefined(int operationIndex, int parameterIndex){
    try {
        const OVertex& operationVertex = _operationNodes[operationIndex];
        return _workflow->hasValueDefined(operationVertex, parameterIndex);
    } catch (std::out_of_range e) {
       return false;
    }
}

/**
 * Returns the values of an operation which have already been defined (a flow has been drawn to it)
 * @param operationIndex the operation to check
 * @return a string of fields which have been defined, seperated by |
 */
QString WorkflowModel::definedValueIndexes(int operationIndex){
    try {
        const OVertex& operationVertex = _operationNodes[operationIndex];
        return _workflow->definedValueIndexes(operationVertex);
    } catch (std::out_of_range e) {
       return "";
    }
}

void WorkflowModel::deleteOperation(int index)
{
    try {
        if ( index < _operationNodes.size()){
            const OVertex& operationVertex = _operationNodes[index];

            // In edges
            std::pair<InEdgeIterator,InEdgeIterator> inIterators = _workflow->getInEdges(operationVertex);
            for (auto &iter = inIterators.first; iter < inIterators.second; ++iter) {
                _workflow->removeOperationFlow(*iter);
            }

            // Out edges
            std::pair<OutEdgeIterator,OutEdgeIterator> outIterators = _workflow->getOutEdges(operationVertex);
            for (auto &iter = outIterators.first; iter < outIterators.second; ++iter) {
                _workflow->removeOperationFlow(*iter);
            }

            _workflow->removeOperation(operationVertex);
            _operationNodes.erase(_operationNodes.begin() + index);

        } else {
            qDebug() << "There are no operations";
        }
    } catch (std::out_of_range e) {
        qDebug() << "False operation";
    }
}

void WorkflowModel::deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd)
{
    OVertex sourceNode = _operationNodes[operationIndex1];
    boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = _workflow->getOutEdges(sourceNode); ei != ei_end; ++ei) {

        OVertex targetNode = _workflow->getTargetOperationNode(*ei);

        NodeProperties npNode = _workflow->nodeProperties(targetNode);
        NodeProperties npTarget = _workflow->nodeProperties(_operationNodes[operationIndex2]);

        if(npNode._operationid == npTarget._operationid)
        {
            EdgeProperties ep = _workflow->edgeProperties(*ei);

            if(ep._outputParameterIndex == indexStart && ep._inputParameterIndex == indexEnd)
            {
                _workflow->removeOperationFlow(*ei);
            }
        }
    }
}

/**
 * Returns the nodes of the workflow
 */
QQmlListProperty<NodePropObject> WorkflowModel::getNodes()
{
    if ( _nodeProps.size() != 0) {
        for(auto *node : _nodeProps)
            delete node;
        _nodeProps.clear();
    }

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = _workflow->getNodeIterators();
    for (auto &iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        NodePropObject *nodeProp = new NodePropObject();
        nodeProp->setProps(_workflow->nodeProperties(*iter), *iter);
        _nodeProps.append(std::move(nodeProp));
    }
    return  QQmlListProperty<NodePropObject>(this, _nodeProps);
}

///**
// * Returns the edges of the node
// */
QQmlListProperty<EdgePropObject> WorkflowModel::getEdges()
{
    if ( _edgeProps.size() != 0) {
        for(auto *edge : _edgeProps)
            delete edge;
        _edgeProps.clear();
    }

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = _workflow->getNodeIterators();
    for (auto &iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        std::pair<OutEdgeIterator,OutEdgeIterator> edgeIterators = _workflow->getOutEdges(*iter);
        for (auto &iter2 = edgeIterators.first; iter2 < edgeIterators.second; ++iter2) {
            EdgePropObject *edgeProp = new EdgePropObject();
            edgeProp->setProps(_workflow->edgeProperties(*iter2), *iter, _workflow->getTargetOperationNode(*iter2));
            _edgeProps.append(std::move(edgeProp));
        }
    }
    return  QQmlListProperty<EdgePropObject>(this, _edgeProps);
}

/**
 * Runs the createMetadata function on the workflow.
 * The workflow will be put in the master catalog and will be usable.
 */

int WorkflowModel::vertex2ItemID(int vertex)
{
    for (int i = 0; i < _operationNodes.size(); ++i) {
        if (_operationNodes[i] == vertex) {
            return i;
        }
    }
    return iUNDEF;
}

void WorkflowModel::store(const QStringList &coordinates)
{
    try {
        for (int i = 0; i< coordinates.size(); i++) {
            QStringList split = coordinates[i].split('|');
            OVertex v = _operationNodes[i];
            NodeProperties props = _workflow->nodeProperties(v);
            props._x = split[0].toInt();
            props._y = split[1].toInt();
            _workflow->updateNodeProperties(v, props);
        }

        _workflow->name(_workflow->name());
        _workflow->connectTo(QUrl("file:///C:/Users/vincent/Desktop/testdata/workflows/" + _workflow->name()), QString("workflow"), QString("stream"), Ilwis::IlwisObject::cmOUTPUT);
        _workflow->createTime(Ilwis::Time::now());
        _workflow->store();
    } catch(const ErrorObject&){

    }
}

void WorkflowModel::load()
{
    //_workflow->connectTo(QUrl("ilwis://internalcatalog/" + _workflow->name() + "_workflow"), QString("workflow"), QString("stream"), Ilwis::IlwisObject::cmINPUT);

    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> nodeIterators = _workflow->getNodeIterators();
    for (auto &iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        _operationNodes.push_back(*iter);
    }
}

void WorkflowModel::createMetadata()
{
    _workflow->createMetadata();
}
