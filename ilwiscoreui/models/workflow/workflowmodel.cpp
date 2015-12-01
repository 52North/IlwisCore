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
            if (value.trimmed().isEmpty()) {
                constantInput->value = QVariant::Invalid;
            } else {
                constantInput->value = value;
            }
        }
    }
}

void WorkflowModel::addOperation(const QString &id)
{
    _workflow->debugWorkflowMetadata();
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
QList<NodePropObject*> WorkflowModel::getNodes()
{
    std::pair<VertexIterator, VertexIterator> nodeIterators = _workflow->getNodeIterators();
    QList<NodePropObject*> *nodeProps = new QList<NodePropObject*>();
    for (auto &iter = nodeIterators.first; iter < nodeIterators.second; ++iter) {
        NodePropObject *nodeProp = new NodePropObject();
        nodeProp->setProps(_workflow->nodeProperties(*iter), *iter);
        nodeProps->append(nodeProp);
    }
    return *nodeProps;
}

///**
// * Returns the edges of the node
// */
//QList<EdgeProperties> WorkflowModel::getEdgesByNode()
//{
//    return *(new QList<EdgeProperties>());
//}

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

void WorkflowModel::createMetadata()
{
    _workflow->createMetadata();
}
