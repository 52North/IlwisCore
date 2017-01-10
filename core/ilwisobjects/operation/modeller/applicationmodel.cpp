#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "workflow.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "model.h"

using namespace Ilwis;

ApplicationModel::ApplicationModel()
{

}

ApplicationModel::ApplicationModel(const QString &name, const QString &description) : Identity(name,i64UNDEF,description)
{

}

void ApplicationModel::store(QDataStream &stream)
{
    Identity::store(stream)    ;
    stream << _analysisName;
}

void ApplicationModel::loadMetadata(QDataStream &stream)
{
    Identity::load(stream);
    stream >> _analysisName;
}

QString ApplicationModel::attachedAnalysis() const
{
    return _analysisName;
}

void ApplicationModel::attachedAnalysis(const QString &name)
{
    _analysisName = name;
}

void ApplicationModel::attachedModel(quint64 modelid)
{
    _modelId = modelid;
}

IModel ApplicationModel::attachedModel()
{
    if ( !_attachedModel.isValid())
        _attachedModel.prepare(_modelId);
    return _attachedModel;
}
