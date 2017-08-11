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
#include "ilwisobjectmodel.h"
#include "symboltable.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "operationExpression.h"
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

    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedOperation READ selectedOperation NOTIFY selectionChanged)
    Q_PROPERTY(double scale  READ scale WRITE scale NOTIFY scaleChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)
    Q_PROPERTY(QVariantMap translation READ translation NOTIFY translationChanged)
    Q_PROPERTY(bool stepMode READ stepMode WRITE stepMode NOTIFY stepModeChanged)
    Q_PROPERTY(int lastOperationNode READ lastOperationNode NOTIFY operationNodeChanged)
    Q_PROPERTY(int currentNode READ currentNode NOTIFY currentNodeChanged)
    Q_PROPERTY(QVariantList outputCurrentOperation READ outputCurrentOperation NOTIFY outputCurrentOperationChanged)
    Q_PROPERTY(qint32 runid READ runid CONSTANT)

    Q_INVOKABLE quint32 addNode(const QString &id, const QVariantMap &parameters);
    Q_INVOKABLE void addFlow(int nodeIdFrom, int nodeIdTo, qint32 inParmIndex, qint32 outParmIndex, int rectFrom, int rectTo);
    Q_INVOKABLE void addConditionFlow(int conditionIdTo, const QString &operationIdFrom, int testIndex, int inParameterIndex, int outParmIndex, int rectFrom, int rectTo);
    Q_INVOKABLE void addTest2Condition(int conditionId, const QString& operationId, const QString& pre, const QString& post);
    Q_INVOKABLE void addCondition2Junction(int conditionId, int junctionId);
    Q_INVOKABLE void setTestValues(int conditionId, int testIndex, int parameterIndex, const QString& value);
    Q_INVOKABLE void setRangeTestValues(int rangeid, const QString& rangeDef);
    Q_INVOKABLE QString testValueDataType(quint32 conditionId, quint32 testIndex, quint32 parameterIndex) const;
    Q_INVOKABLE QString testValue(int conditionId, int testIndex, int parameterIndex);
    Q_INVOKABLE QVariantList getTestParameters(int nodeId, int testIndex);
    Q_INVOKABLE void updateOperationParameters(quint32 nodeid, int paramterIndex, const QString& txt);
    Q_INVOKABLE void addJunctionFlows(int junctionIdTo, const QString& operationIdFrom, int paramterIndex, int rectFrom, int rectTo, bool truecase);
    Q_INVOKABLE void changeBox(int nodeId, int x, int y, int w, int h);
    Q_INVOKABLE bool hasValueDefined(int nodeId, int parameterIndex);
    Q_INVOKABLE int operationInputParameterCount(int nodeId);
    Q_INVOKABLE int operationOutputParameterCount(int nodeId);
    Q_INVOKABLE int freeInputParameterCount(int nodeId);
    Q_INVOKABLE bool usableLink(int sourceNodeId, int targetNodeId, int sourceParmIndex, int targetParmIndex);
    Q_INVOKABLE void removeNode(int nodeId);
    Q_INVOKABLE void deleteFlow(int nodeId, int parameterIndex);
    Q_INVOKABLE QVariantList getNodes();
    Q_INVOKABLE QVariantMap getNode(int nodeId);
    Q_INVOKABLE QVariantList getTests(int conditionId) const;
    Q_INVOKABLE void createMetadata();
    Q_INVOKABLE QVariantList propertyList();
    Q_INVOKABLE QString generateScript(const QString &type, const QString &parameters);
    Q_INVOKABLE void toggleStepMode();
    Q_INVOKABLE void nextStep();
    Q_INVOKABLE void store(const QString &container, const QString &name);
    Q_INVOKABLE void setFixedValues(qint32 nodeid, const QString& formValues);
    Q_INVOKABLE bool isValidNode(qint32 nodeId, const QString &part) const;
    Q_INVOKABLE void translateObject(int x, int y, bool relative);
    QQmlListProperty<IlwisObjectModel> selectedOperation();
    Q_INVOKABLE void selectOperation(const QString& id);
    Q_INVOKABLE bool collapsed(int nodeid) const;
    Q_INVOKABLE void collapsed(int nodeid, bool yesno);
    Q_INVOKABLE void setNodeProperty(int nodeid, int paramindex, const QString& property, const QString &value);
    Q_INVOKABLE void stopStepMode();
    Q_INVOKABLE void stopExecution();

    QString modelType() const;




signals:
    void sendMessage(const QString& type, const QString& subtype, const QVariantMap& parameters);
    void selectionChanged();
    void scaleChanged();
    void translationChanged();
    void validChanged();
    void outputCurrentOperationChanged();
    void operationNodeChanged();
    void stepModeChanged();
    void currentNodeChanged();

public slots:
    void acceptMessage(const QString &type, const QString &subtype, const QVariantMap &parameters);
private:
    Ilwis::IWorkflow _workflow;

    bool _stepMode = false;
    OperationExpression _expression;
    quint32 _runid;
    qint32 _lastOperationNode = -1;
    qint32 _currentNode = -1;
    static quint32 _baserunid;
    QVariantMap _outputsCurrentOperation;
    QVariantList _outputs;

    QVariantList outputCurrentOperation();
    int lastOperationNode() const;
    int currentNode() const;

    QList<IlwisObjectModel*> _selectedOperation;
    QVariantMap getParameter(const SPWorkFlowNode& node, int i);
    QVariantMap translation() const;
    bool isValid() const;
    double scale() const;
    void scale(double s);


    QString nodetype2string(WorkFlowNode::NodeTypes ntype) const;
    WorkFlowNode::NodeTypes string2nodetype(const QString& ntype) const;
    void stepMode(bool yesno);
    bool stepMode() const;
    qint32 runid() const;
};


typedef QQmlListProperty<WorkflowModel> QMLWorkflowList;

Q_DECLARE_METATYPE(QMLWorkflowList)


#endif // WORKFLOWMODEL_H
