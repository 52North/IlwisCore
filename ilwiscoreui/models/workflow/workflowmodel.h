#ifndef WORKFLOWMODEL_H
#define WORKFLOWMODEL_H

#include "models/operationmodel.h"
#include <QPoint>
#include <QMap>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "ilwistypes.h"
#include "operationmetadata.h"
#include "workflow.h"
#include "nodepropobject.h"
#include "edgepropobject.h"
#include <QQmlListProperty>

#include "rastercoverage.h"
using namespace Ilwis;
namespace Ilwis {
class OperationMetaData;
typedef IlwisData<OperationMetaData> IOperationMetaData;
}
/**
 * Adds workflow specific accessors to an OperationModel.
 *
 * @brief The WorkflowModel class
 */
class ILWISCOREUISHARED_EXPORT WorkflowModel: public OperationModel
{
     Q_OBJECT
public:
    WorkflowModel();
    ~WorkflowModel();
    explicit WorkflowModel(const Ilwis::Resource &source, QObject *parent=0);

    Q_PROPERTY(QQmlListProperty<NodePropObject>  nodes READ getNodes CONSTANT)
    Q_PROPERTY(QQmlListProperty<EdgePropObject>  edges READ getEdges CONSTANT)

    Q_INVOKABLE void asignConstantInputData(QString inputData, int operationIndex);
    Q_INVOKABLE void addOperation(const QString& id);
    Q_INVOKABLE void addFlow(int indexStart, int operationIndex2, const QVariantMap &flowpoints, int outRectIndex, int inRectIndex);
    Q_INVOKABLE void deleteOperation(int index);
    Q_INVOKABLE void deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd);

    Q_INVOKABLE bool hasValueDefined(int operationIndex, int parameterIndex);

    Q_INVOKABLE QString definedValueIndexes(int operationIndex);

    QQmlListProperty<NodePropObject> getNodes() ;
    QQmlListProperty<EdgePropObject> getEdges();
    Q_INVOKABLE int vertex2ItemID(int vertex);

    Q_INVOKABLE void store(const QStringList &coordinates);
    Q_INVOKABLE void load();

    Q_INVOKABLE void createMetadata();

private:
    Ilwis::IWorkflow _workflow;
    std::vector<Ilwis::OVertex> _operationNodes;
    QList<NodePropObject *> _nodeProps;
    QList<EdgePropObject *> _edgeProps;
};


typedef QQmlListProperty<WorkflowModel> QMLWorkflowList;

Q_DECLARE_METATYPE(QMLWorkflowList)


#endif // WORKFLOWMODEL_H
