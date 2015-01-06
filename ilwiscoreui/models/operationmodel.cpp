#include <QString>
#include <QFileInfo>
#include <QUrl>
#include "kernel.h"
#include "connectorinterface.h"
#include "resource.h"
#include "ilwisobject.h"
#include "mastercatalog.h"
#include "resourcemodel.h"
#include "operationmodel.h"

using namespace Ilwis;

OperationModel::OperationModel()
{
}

OperationModel::OperationModel(const Ilwis::Resource& source, QObject *parent) : ResourceModel(source, parent)
{
    if ( item().hasProperty("longname"))
        _displayName =  item()["longname"].toString();
    else
        _displayName = item().name();
}


QString OperationModel::inputparameterName(quint32 index) const
{
    return property("pin_" + QString::number(index + 1) + "_name").toString();

}

QString OperationModel::inputparameterType(quint32 index) const
{
    QVariant var = property("pin_" + QString::number(index + 1) + "_type");
    quint64 ilwtype = var.toULongLong();
    return TypeHelper::type2HumanReadable(ilwtype);
}

QString OperationModel::inputparameterDescription(quint32 index) const
{
    return property("pin_" + QString::number(index + 1) + "_desc").toString();
}

QString OperationModel::outputparameterName(quint32 index) const
{
    return property("pout_" + QString::number(index + 1) + "_name").toString();
}

QString OperationModel::outputparameterType(quint32 index) const
{
    QVariant var = property("pout_" + QString::number(index + 1) + "_type");
    quint64 ilwtype = var.toULongLong();
    return TypeHelper::type2HumanReadable(ilwtype);
}

QString OperationModel::outputparameterDescription(quint32 index) const
{
    return property("pout_" + QString::number(index + 1) + "_desc").toString();
}

QString OperationModel::syntax() const
{
    return property("syntax").toString();
}

QString OperationModel::keywords() const
{
    return  property("keywords").toString();
}

QVariant OperationModel::property(const QString &name) const
{
    if ( item().hasProperty(name))
        return item()[name];
    return sUNDEF;
}

