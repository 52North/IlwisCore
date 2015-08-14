#include <QRegExp>

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
        _minInputCountParameters = parts.first().toInt();
        quint16 maxCountParameters = parts.back().toInt();
        parmfromResource(maxCountParameters,"pin");
    }
    pcount = resource["outparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        _minOutputCountParameters = parts.first().toInt();
        quint16 maxCountParameters = parts.back().toInt();
        parmfromResource(maxCountParameters,"pout");
    }
}

OperationMetaData::~OperationMetaData()
{
}

void OperationMetaData::parmfromResource(int n, const QString& base)
{
    //QStringList required;
    //QStringList optional;
    //parametersFromSyntax(required, optional);

    for(int i=0; i < n; ++i) {
        //bool isOptional = required.size() < i+1;
        QString parmBase = base + QString("_%1_").arg(i+1);

        bool ok;
        quint64 tp = source()[parmBase + "type"].toLongLong(&ok);
        if (!ok)
            tp = i64UNDEF;
        QString name = source()[parmBase + "name"].toString();
        QString domainName = source()[parmBase + "domain"].toString();
        QString description = source()[parmBase + "desc"].toString();
        OperationParameter::ParameterKind kind = base == "pin"
                ? OperationParameter::ptINPUT
                : OperationParameter::ptOUTPUT;
        SPOperationParameter parameter = newParameter(kind,name,tp,domainName);
        //parameter->optional(isOptional);
    }
}

void OperationMetaData::parametersFromSyntax(QStringList &required, QStringList &optional)
{
    QRegExp argumentsrx("^.*\\((.*)\\)(\\s+)?$");
    int argIdx = argumentsrx.indexIn(source()["syntax"].toString());
    QString arguments =  argIdx != -1 ? argumentsrx.cap(1) : "";

    QRegExp requiredrx("^(.*)\\[");
    int reqIdx = requiredrx.indexIn(arguments);
    QString requireds = reqIdx != -1 ? requiredrx.cap(1) : "";

    QRegExp optionalsrx("^.*\\[(.*)\\](\\s+)?$");
    int optIdx = optionalsrx.indexIn(arguments);
    QString optionals = optIdx != -1 ? optionalsrx.cap(1) : "";

    QRegExp commaWithWhiteSpaces("\\s*,\\s*");
    required << requireds.split(commaWithWhiteSpaces, QString::SkipEmptyParts);
    optional << optionals.split(commaWithWhiteSpaces, QString::SkipEmptyParts);
}

IlwisTypes OperationMetaData::ilwisType() const
{
    return itOPERATIONMETADATA;
}

std::vector<SPOperationParameter> OperationMetaData::getInputParameters() const
{
    return _inputParameters;
}

std::vector<SPOperationParameter> OperationMetaData::getOutputParameters() const
{
    return _outputParameters;
}

QString OperationMetaData::getNamespace() const
{
    return source()["namespace"].toString();
}

QString OperationMetaData::getLongName() const
{
    return source()["longname"].toString();
}

QStringList OperationMetaData::getKeywords() const
{
    return source()["keywords"].toStringList();
}

void OperationMetaData::setNamespace(const QString &nspace)
{
    source().addProperty("namespace", nspace);
}

void OperationMetaData::setLongName(const QString &longName)
{
    source().addProperty("longname", longName);
}

void OperationMetaData::setKeywords(const QStringList &keywords)
{
    source().addProperty("keywords", keywords);
}

void OperationMetaData::clearInputs()
{
    _inputParameters.clear();
}

void OperationMetaData::clearOutputs()
{
    _outputParameters.clear();
}

quint16 OperationMetaData::minInputCountParameters()
{
    return _minInputCountParameters;
}

quint16 OperationMetaData::minOutputCountParameters()
{
    return _minOutputCountParameters;
}

SPOperationParameter OperationMetaData::newParameter(OperationParameter::ParameterKind kind, const QString &name, IlwisTypes type, const QString &domain, const QString &description, bool optional)
{
    return addParameter(SPOperationParameter(new OperationParameter(kind, name, type, domain, description, optional)));
}

SPOperationParameter OperationMetaData::addParameter(SPOperationParameter parameter)
{
    bool input = parameter->kind() == OperationParameter::ptINPUT;
    if (input) {
        parameter->index(_inputParameters.size());
        _inputParameters.push_back(parameter);
    } else {
        parameter->index(_outputParameters.size());
        _outputParameters.push_back(parameter);
    }
    return parameter;
}

OperationParameter::OperationParameter(const OperationParameter &operationParameter):
    Identity(name()),
    _index(operationParameter._index),
    _kind(operationParameter._kind),
    _type(operationParameter._type),
    _domainName(operationParameter._domainName),
    _optional(operationParameter._optional)
{
    setDescription(operationParameter.description());
}

IlwisTypes OperationParameter::type() const
{
    return _type;
}

OperationParameter::ParameterKind OperationParameter::kind() const
{
    return _kind;
}

quint16 OperationParameter::index() const
{
    return _index;
}

QString OperationParameter::domainName() const
{
    return _domainName;
}

bool OperationParameter::isOptional() const
{
    return _optional;
}

void OperationParameter::index(quint16 index)
{
    _index = index;
}

void OperationParameter::optional(bool optional)
{
    _optional = optional;
}

void OperationParameter::addToResource(Resource resource) const
{
    QString prefix;
    bool input = kind() == OperationParameter::ptINPUT;
    if (input) {
        prefix = "pin_" + QString::number(index()+1) + "_";
    } else {
        prefix = "pout_" + QString::number(index()+1) + "_";
    }
    resource.addProperty(prefix + "type", _type);
    resource.addProperty(prefix + "name", name());
    resource.addProperty(prefix + "desc", description());
}

OperationParameter::OperationParameter(OperationParameter::ParameterKind kind, const QString &name, IlwisTypes type, const QString &domain, const QString &description, bool optional) :
    Identity(name),
    _kind(kind),
    _type(type),
    _domainName(domain),
    _optional(optional)
{
    setDescription(description);
}

//-------------------------------------------------------------------------------
OperationResource::OperationResource(const QUrl &inurl, const QString &nmspace) :  Resource(inurl,itSINGLEOPERATION)
{
    prepare();
    QString urls = url().toString() + "=" + QString::number(id());
    setUrl(urls);
    addProperty("namespace",nmspace);
}

OperationResource::OperationResource(const QUrl &inurl, const IlwisTypes &type, const QString &nmspace) : Resource(inurl,type)
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
    addProperty(prefix + "optional", false);
}

void OperationResource::addOptionalInParameter(quint32 order, IlwisTypes type, const QString &name, const QString &description)
{
    QString prefix = "pin_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", name);
    addProperty(prefix + "desc", description);
    addProperty(prefix + "optional", true);
}

void OperationResource::addOutParameter(quint32 order, IlwisTypes type, const QString &nme, const QString &description)
{
    QString prefix = "pout_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", nme);
    addProperty(prefix + "optional", false);
}

void OperationResource::addOptionalOutParameter(quint32 order, IlwisTypes type, const QString &name, const QString &description)
{
    QString prefix = "pout_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", name);
    addProperty(prefix + "desc", description);
    addProperty(prefix + "optional", true);
}

void OperationResource::setKeywords(const QString& words) {

    addProperty("keyword", words.trimmed().toLower());
}


