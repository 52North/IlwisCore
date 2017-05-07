#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "operationExpression.h"
#include "workflow.h"
#include "applicationmodel.h"
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

void AnalysisPattern::loadMetadata(QDataStream &stream)
{
    Identity::load(stream)    ;
}

void AnalysisPattern::loadData(QDataStream &stream)
{
    //TODO
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
