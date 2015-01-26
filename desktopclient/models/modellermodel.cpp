#include "kernel.h"
#include "modellermodel.h"

using namespace Ilwis;

ModellerModel::ModellerModel(QObject *parent) :
    QObject(parent)
{
    addWorkflow("GCL_INT__12.mpr", "Model of GCL_INT__12.mpr");
    addWorkflow("woredas.shp", "Model of woredas.shp");
    addWorkflow("average_monthly_temperature_june_7.mpr", "Model of average_monthly_temperature_june_7.mpr");
}

QMLModellerList ModellerModel::workflows()
{
    return  QQmlListProperty<WorkflowModel>(this, _models);
}

void ModellerModel::addWorkflow(const QString& name, const QString& description)
{
    _models.insert(_models.size(), new WorkflowModel(name, description, this));

}

qint64 ModellerModel::count()
{
    return _models.size();
}

WorkflowModel* ModellerModel::get(quint32 index)
{
    return _models[index];
}

void ModellerModel::removeWorkflow(quint32 index)
{
    if ( 0 <= index < _models.size()) {
        _models.removeAt(index);
    }
}

//---------------------------------------------------
WorkflowModel::WorkflowModel()
{

}

WorkflowModel::WorkflowModel(const QString &name, const QString &description, QObject *parent) : QObject(parent)
{
    _name = name;
    _description = description;
}

QString WorkflowModel::modelName() const
{
    return  _name;
}

QString WorkflowModel::name() const
{
    return  _name;
}

QString WorkflowModel::description() const
{
    return _description;
}
