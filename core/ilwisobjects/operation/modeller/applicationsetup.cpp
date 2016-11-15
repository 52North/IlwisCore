#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "applicationsetup.h"
#include "analysispattern.h"
#include "model.h"

using namespace Ilwis;

ModelApplication::ModelApplication()
{

}

void ModelApplication::store(QDataStream &stream)
{
    Identity::store(stream)    ;
    stream << _analysisName;
}

void ModelApplication::load(QDataStream &stream)
{
    Identity::load(stream);
    stream >> _analysisName;
}

QString ModelApplication::attachedAnalysis() const
{
    return _analysisName;
}

void ModelApplication::attachedAnalysis(const QString &name)
{
    _analysisName = name;
}

void ModelApplication::attachedModel(quint64 modelid)
{
    _modelId = modelid;
}

IModel ModelApplication::attachedModel()
{
    if ( !_attachedModel.isValid())
        _attachedModel.prepare(_modelId);
    return _attachedModel;
}
