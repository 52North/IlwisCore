#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"

using namespace Ilwis;

OperationMetaData::OperationMetaData()
{
}

OperationMetaData::OperationMetaData(const Resource &resource) : IlwisObject(resource)
{
    QString pcount = resource["inparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        parmfromResource(resource,parts.back().toInt(),"pin");
    }
    pcount = resource["outparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        parmfromResource(resource,parts.back().toInt(),"pout");
    }
    _namespace = resource["namespace"].toString();
    _longName = resource["longname"].toString();
    _keywords = resource["keywords"].toStringList();
}

OperationMetaData::~OperationMetaData()
{
}

void OperationMetaData::parmfromResource(const Resource &resource, int n, const QString& base) {
    for(int i=0; i < n; ++i)    {
        QString parmBase = base + QString("_%1_").arg(i+1);

        bool ok;
        quint64 tp = resource[parmBase + "type"].toLongLong(&ok);
        if (!ok)
            tp = i64UNDEF;
        QString name = resource[parmBase + "name"].toString();
        QString domainName = resource[parmBase + "domain"].toString();
        newParameter(OperationParameter::ptINPUT,name,tp,domainName);
    }
}

IlwisTypes OperationMetaData::ilwisType() const
{
    return itOPERATIONMETADATA;
}

QString OperationMetaData::npace() const
{
    return _namespace;
}


UPOperationParameter& OperationMetaData::newParameter(OperationParameter::ParameterNature kind, const QString &name, IlwisTypes type, const QString &domain)
{
    bool useInput = kind == OperationParameter::ptINPUT;
    for(UPOperationParameter& parm : (useInput ? _inputParameters : _outputParameters)){
        if ( parm->nature() == kind && parm->type() == type)
            return parm;
    }
    if ( useInput)
        _inputParameters.push_back( UPOperationParameter(new OperationParameter(_inputParameters.size(), kind, name, type, domain)));
    else
        _outputParameters.push_back( UPOperationParameter(new OperationParameter(_outputParameters.size(), kind, name, type, domain)));

    return kind == OperationParameter::ptINPUT ? _inputParameters.back() : _outputParameters.back();
}


IlwisTypes OperationParameter::type() const
{
    return _type;
}

OperationParameter::ParameterNature OperationParameter::nature() const
{
    return _nature;
}

quint16 OperationParameter::index() const
{
    return _index;
}

QString OperationParameter::domainName() const
{
    return _domainName;
}

OperationParameter::OperationParameter(quint16 index, OperationParameter::ParameterNature kind, const QString &name, IlwisTypes type, const QString &domain) :
    Identity(name),
    _index(index),
    _nature(kind),
    _type(type),
    _domainName(domain)
{
}

//-------------------------------------------------------------------------------
OperationResource::OperationResource(const QUrl &inurl, const QString &nmspace) : Resource(inurl,itOPERATIONMETADATA)
{
    prepare();
    QString urls = url().toString() + "=" + QString::number(id());
    setUrl(urls);
    addProperty("namespace",nmspace);
}

void OperationResource::setLongName(const QString &longname)
{
    addProperty("longname",longname);
}

void OperationResource::setSyntax(const QString &syntax)
{
    addProperty("syntax",syntax);
}

void OperationResource::setInParameterCount(const std::vector<quint32> &counts)
{
    QString lst;
    for(quint32 num : counts) {
        if ( lst != "")
            lst += "|";
        lst += QString::number(num);
    }
    addProperty("inparameters",lst);
}

void OperationResource::setOutParameterCount(const std::vector<quint32> &counts)
{
    QString lst;
    for(quint32 num : counts) {
        if ( lst != "")
            lst += "|";
        lst += QString::number(num);
    }
    addProperty("outparameters",lst);
}

void OperationResource::addInParameter(quint32 order, IlwisTypes type, const QString &nme, const QString &description)
{
    QString prefix = "pin_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", nme);
    addProperty(prefix + "desc", description);
}

void OperationResource::addOutParameter(quint32 order, IlwisTypes type, const QString &nme, const QString &description)
{
    QString prefix = "pout_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", nme);
    addProperty(prefix + "desc", description);
}

void OperationResource::setKeywords(const QString& words) {

    addProperty("keyword", words.trimmed().toLower());
}


