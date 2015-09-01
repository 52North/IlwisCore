#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"

using namespace Ilwis;

WorkflowModel::WorkflowModel()
{
}

WorkflowModel::WorkflowModel(const Ilwis::Resource &source, QObject *parent) : OperationModel(source, parent)
{

}

void WorkflowModel::addOperation(int index, const QString &id)
{
    bool ok;
    quint64 opid = id.toULongLong(&ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("Invalid operation id used in workflow %1")).arg(name()));
        return ;
    }
    auto vertex = _workflow.addOperation({opid});
    _operationNodes[index] = vertex;

}

void WorkflowModel::addFlow(int operationIndex1, int operationIndex2, const QVariantMap& flowpoints)
{
    if ( operationIndex1 >= 0 && operationIndex2 >= 0 && flowpoints.size() == 2) {
        auto fromVertexIter = _operationNodes.find(operationIndex1);
        auto toVertexIter  = _operationNodes.find(operationIndex2);
        if ( fromVertexIter != _operationNodes.end() && toVertexIter != _operationNodes.end()){
            const OVertex& fromOperationVertex = (*fromVertexIter).second;
            const OVertex& toOperationVertex = (*toVertexIter).second;
            int outParamIndex = flowpoints["fromParameterIndex"].toInt();
            int inParamIndex = flowpoints["toParameterIndex"].toInt();
            EdgeProperties flowPoperties{outParamIndex, inParamIndex};
            _workflow.addOperationFlow(fromOperationVertex,toOperationVertex,flowPoperties);

        }
    }
}

bool WorkflowModel::hasValueDefined(int operationindex, int parameterindex){
    auto vertexIter = _operationNodes.find(operationindex);
    if ( vertexIter != _operationNodes.end()){
        const OVertex& operationVertex = (*vertexIter).second;
        return _workflow.hasValueDefined(operationVertex, parameterindex);
    }
    return false;
}

void WorkflowModel::deleteOperation(int index)
{
//    if ( index < _operations.size()){
//        _operations.erase(_operations.begin() + index);
//        bool again = true;
//        while(again){
//            for(auto iter = _flows.begin(); iter != _flows.end(); ++iter){
//                again = false;
//                if ( (*iter)._beginOperation == index || (*iter)._endOperation == index){
//                    _flows.erase(iter);
//                    again = true;
//                    break;
//                }
//            }
//        }
//    }
}

void WorkflowModel::deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd)
{
//     for(auto iter = _flows.begin(); iter != _flows.end(); ++iter){
//         if ( (*iter)._beginOperation == operationIndex1 && (*iter)._endOperation == operationIndex2){
//             if ( (*iter)._inParam == indexStart && (*iter)._outParam == indexEnd){
//                _flows.erase(iter);
//                return;
//             }
//         }

//     }
}
