#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "applicationsetup.h"
#include "analysispattern.h"
#include "model.h"

using namespace Ilwis;

AnalysisPattern::AnalysisPattern()
{

}

AnalysisPattern::AnalysisPattern(const QString &name, const QString &description) : Identity(name,i64UNDEF, description)
{

}

void AnalysisPattern::store(QDataStream &stream)
{
    Identity::store(stream);
}

void AnalysisPattern::load(QDataStream &stream)
{
    Identity::load(stream)    ;
}

void AnalysisPattern::attachedModel(quint64 modelid)
{
    _modelId = modelid;
}

IModel AnalysisPattern::attachedModel()
{
    if ( !_attachedModel.isValid())
        _attachedModel.prepare(_modelId);
    return _attachedModel;
}
