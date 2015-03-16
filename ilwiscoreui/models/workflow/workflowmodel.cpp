
#include "kernel.h"
#include "workflowmodel.h"

using namespace Ilwis;
using namespace boost;



WorkflowModel::WorkflowModel(QObject *parent) :
    QObject(parent)
{

}

WorkflowModelList WorkflowModel::workflows()
{

}

void WorkflowModel::workflowsChanged()
{
    MESSAGE0("workflows changed");
}





WorkflowOperation::WorkflowOperation(const OperationModel &op, QObject *parent)
{

}
