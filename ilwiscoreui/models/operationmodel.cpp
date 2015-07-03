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

OperationModel::OperationModel(quint64 id, QObject *parent) : ResourceModel(mastercatalog()->id2Resource(id), parent)
{

}


QString OperationModel::inputparameterName(quint32 index) const
{
    return getProperty("pin_" + QString::number(index + 1) + "_name");

}

QString OperationModel::inputparameterType(quint32 index) const
{
    quint64 ilwtype = getProperty("pin_" + QString::number(index + 1) + "_type").toULongLong();
    QString type;
    for(quint64 i =0; i < 64; ++i){
       quint64 result = 1 << i;
        if ( hasType(ilwtype, result)) {
            if ( type != "")
                type += ",";
            type += TypeHelper::type2HumanReadable(result);
        }
        if ( result > ilwtype)
            break;
    }
    return type;
}

QString OperationModel::inputparameterDescription(quint32 index) const
{
    return getProperty("pin_" + QString::number(index + 1) + "_desc");
}

QString OperationModel::outputparameterName(quint32 index) const
{
    return getProperty("pout_" + QString::number(index + 1) + "_name");
}

QString OperationModel::outputparameterType(quint32 index) const
{
    quint64 ilwtype = getProperty("pout_" + QString::number(index + 1) + "_type").toULongLong();
    return TypeHelper::type2HumanReadable(ilwtype);
}

QString OperationModel::outputparameterDescription(quint32 index) const
{
    return getProperty("pout_" + QString::number(index + 1) + "_desc");
}

QString OperationModel::syntax() const
{
    return getProperty("syntax");
}

QString OperationModel::keywords() const
{
    return  getProperty("keyword");
}

int OperationModel::maxParameterCount(bool inputCount) const
{
    QString inParams = getProperty(inputCount ? "inparameters" : "outparameters");
    QStringList parts = inParams.split("|");
    int maxp = 0;
    for(QString p : parts){
        maxp = std::max(maxp , p.toInt());
    }
    return maxp;
}

QStringList OperationModel::inParamNames() const
{
    int maxp = maxParameterCount(true);
    QStringList names;
    for(int i = 0; i < maxp; ++i){
        names.append(inputparameterName(i));
    }
    return names;
}

QStringList OperationModel::outParamNames() const
{
    int maxp = maxParameterCount(false);
    QStringList names;
    for(int i = 0; i < maxp; ++i){
        names.append(outputparameterName(i));
    }
    return names;
}

QString OperationModel::getProperty(const QString &name) const
{
    if ( item().hasProperty(name))
        return item()[name].toString();
    return sUNDEF;
}


