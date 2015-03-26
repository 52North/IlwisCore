
#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"

#include "workflow.h"

using namespace Ilwis;

Workflow::Workflow() : OperationMetaData()
{
}

Workflow::Workflow(const Resource &resource): OperationMetaData(resource)
{

}

Workflow::~Workflow()
{

}

quint64 Workflow::init()
{

}

IlwisTypes Workflow::ilwisType() const
{
    return itWORKFLOW;
}
