#ifndef WORKFLOWMODEL_H
#define WORKFLOWMODEL_H

#include "models/operationmodel.h"
#include <QQmlListProperty>

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
    explicit WorkflowModel(const Ilwis::Resource &source, QObject *parent=0);

    Q_INVOKABLE void addOperation(int index, const QString& id);
    Q_INVOKABLE void addFlow(int indexStart, int operationIndex2, int outParameterIndex, int intParameterIndex);
    Q_INVOKABLE void deleteOperation(int index);
    Q_INVOKABLE void deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd);

private:
    struct Flow{
        Flow(int b, int e, int out, int in) : _beginOperation(b), _endOperation(e), _outParam(out), _inParam(in){}
        int _beginOperation;
        int _endOperation;
        int _outParam;
        int _inParam;
    };

    std::vector<Ilwis::IOperationMetaData> _operations;
    std::vector<Flow> _flows;
};


typedef QQmlListProperty<WorkflowModel> QMLWorkflowList;

Q_DECLARE_METATYPE(QMLWorkflowList)

#endif // WORKFLOWMODEL_H
