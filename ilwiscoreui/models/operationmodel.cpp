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

QString OperationModel::inputparameterTypeNames(quint32 index) const
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

QString OperationModel::inputparameterType(quint32 index) const
{
    return getProperty("pin_" + QString::number(index + 1) + "_type");
}

QString OperationModel::inputparameterDescription(quint32 index) const
{
    QString ret = getProperty("pin_" + QString::number(index + 1) + "_desc");
    if ( ret == sUNDEF)
        return "";
    return ret;
}

QString OperationModel::outputparameterName(quint32 index) const
{
    return getProperty("pout_" + QString::number(index + 1) + "_name");
}

QString OperationModel::outputparameterTypeNames(quint32 index) const
{
    quint64 ilwtype = getProperty("pout_" + QString::number(index + 1) + "_type").toULongLong();
    return TypeHelper::type2HumanReadable(ilwtype);
}

QString OperationModel::outputparameterType(quint32 index) const
{
    return getProperty("pout_" + QString::number(index + 1) + "_type");
}

QString OperationModel::outputparameterDescription(quint32 index) const
{
    QString ret = getProperty("pout_" + QString::number(index + 1) + "_desc");
    if ( ret == sUNDEF)
        return "";
    return ret;
}

QString OperationModel::syntax() const
{
    QString syn = getProperty("syntax");
    if ( syn.indexOf("ilwis://")!= -1){
        syn = syn.mid(QString("ilwis://operations/").size());
    }
    return syn;
}

QString OperationModel::keywords() const
{
    QString kw =   getProperty("keyword");
    if ( kw == sUNDEF)
        return "";
    return kw;
}

QString OperationModel::provider() const
{
    QString kw =   getProperty("namespace");
    if ( kw == sUNDEF)
        return "";
    return kw;
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

QString OperationModel::inParameterCount() const
{
    QString inParams = getProperty("inparameters");
    if ( inParams != sUNDEF){
        inParams.replace("|"," or ");
    }else
        inParams = "";
    return inParams;
}

QString OperationModel::outParameterCount() const
{
    QString outParams = getProperty("outparameters");
    if ( outParams != sUNDEF){
        outParams.replace("|"," or ");
    }else
        outParams = "";
    return outParams;
}

QStringList OperationModel::inParameterIconList() const
{
    int mx = maxParameterCount(true);
    QStringList icons;
    for(int i = 0; i < mx; ++i){
        QString p = "pin_" + QString::number(i+1) + "_type";
        IlwisTypes type = item()[p].toULongLong();
        if ( type == iUNDEF)
            continue;
        icons.append(iconPath(type));

    }
    return icons;
}

QStringList OperationModel::outParameterIconList() const
{
    int mx = maxParameterCount(false);
    QStringList icons;
    for(int i = 0; i < mx; ++i){
        QString p = "pin_" + QString::number(i+1) + "_type";
        IlwisTypes type = item()[p].toULongLong();
        if ( type == iUNDEF)
            continue;
        icons.append(iconPath(type));

    }
    return icons;
}

QString OperationModel::getProperty(const QString &name) const
{
    if ( item().hasProperty(name))
        return item()[name].toString();
    return sUNDEF;
}

bool OperationModel::needChoice(OperationModel *other) const
{
    const auto check = [](const QString& type, const OperationModel *operation) ->bool const {
        bool ok = false;
        QString parms = operation->getProperty(type);
        QStringList parts = parms.split("|");
        for(const auto& part : parts){
            if ( part.toInt() > 1) {
                ok = true;
                break;
            }
        }
        return ok;
    };

    if (!other)
        return false;
    if ( check("inparameters", other))
        return true;
    return check("outparameters",this);
}

bool OperationModel::isLegalFlow(OperationModel *from, OperationModel *to) const
{

    if ( !to || !from)
        return false;

    int outParamCount = from->outParamNames().size();

    for(int i = 0; i < outParamCount; i++)
    {
        if(to->parameterIndexes(from->outputparameterType(i), true).size() != 0)
        {
            return true;
        }
    }
    //TODO: Error gooien
    return false;
}

QStringList OperationModel::parameterIndexes(const QString &typefilter, bool fromOperation)
{
    QStringList indexes;
    IlwisTypes tp1 = typefilter.toULongLong();

    bool found = true;
    for(int i = 0; i < maxParameterCount(!fromOperation); ++i){
        if ( typefilter != ""){
            found = false;
            if ( typefilter != ""){
                quint64 tp2 = getProperty((fromOperation ? "pout_" : "pin_") + QString::number(i + 1) + "_type").toULongLong();
                if ( hasType(tp1, tp2)){
                    found = true;
                }
            }
        }
        if ( found)
        {
            // for from flow you want to get the output names of the from operation and vice versa for the other case
            indexes.push_back(QString::number(i) + ": " + (fromOperation ? outputparameterName(i) : inputparameterName(i)));
        }
    }
    return indexes;
}


