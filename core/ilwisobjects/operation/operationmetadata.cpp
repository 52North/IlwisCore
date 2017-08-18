#include <QRegExp>

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "connectorinterface.h"
#include "operationmetadata.h"

using namespace Ilwis;

OperationMetaData::OperationMetaData()
{
}

OperationMetaData::OperationMetaData(const Resource &resource) : IlwisObject(resource)
{
}

OperationMetaData::~OperationMetaData()
{
}

bool OperationMetaData::prepare(const IOOptions&opt)
{
    QString pcount = resource()["inparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        _minInputCountParameters = parts.first().toInt();
        quint16 maxCountParameters = parts.back().toInt();
        parmfromResource(maxCountParameters,"pin");
    }
    pcount = resource()["outparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        _minOutputCountParameters = parts.first().toInt();
        quint16 maxCountParameters = parts.back().toInt();
        parmfromResource(maxCountParameters,"pout");
    }
    return true;
}

void OperationMetaData::parmfromResource(int n, const QString& base)
{
    QStringList required;
    QStringList optional;
    QStringList allParameterNames;
    parametersFromSyntax(required, optional);
    allParameterNames << required << optional;
    if (allParameterNames.isEmpty()) {
        //WARN1("syntax parsing did not result in any parameters: %1", source()["syntax"].toString());
        return;
    }

    Resource res = resource();
    for(int i=0; i < n; ++i) {
        if ( i < allParameterNames.size()){
            //bool isOptional = required.size() < i+1;
            QString parmBase = base + QString("_%1_").arg(i+1);

            bool ok;
            quint64 tp = res[parmBase + "type"].toLongLong(&ok);
            if (!ok) {
                tp = i64UNDEF;
            }
            QString name = res[parmBase + "name"].toString();
            QString domainName = res[parmBase + "domain"].toString();
            QString description = res[parmBase + "desc"].toString();
            bool optional = res[parmBase + "optional"].toBool();
            bool needsQuotes = res[parmBase + "needsquotes"].toBool();
            int linkedinput =  res.hasProperty("outputisinput") ? res["outputisinput"].toInt() : -1;

            QString term;
            OperationParameter::ParameterKind kind = OperationParameter::ptOUTPUT;
            if (base == "pin") {
                kind = OperationParameter::ptINPUT;
                term = allParameterNames.at(i);
                connector()->addProperty(base + "term", term);
            } else {
                term = ""; // formal output term not part of syntax
            }

            addParameter(newParameter(kind,term,name,tp,domainName,description,optional,needsQuotes,linkedinput));
        }
    }
}

void OperationMetaData::parametersFromSyntax(QStringList &required, QStringList &optional)
{

    QRegExp argumentsrx("^.*\\((.*)\\)(\\s*)?$");
    int argIdx = argumentsrx.indexIn(resource()["syntax"].toString());
    QString arguments =  argIdx != -1 ? argumentsrx.cap(1) : "";

    QRegExp requiredrx("(.*)\\[");
    int reqIdx = requiredrx.indexIn(arguments);
    QString requireds = reqIdx != -1 ? requiredrx.cap(1) : arguments;

    QRegExp optionalsrx("^.*\\[(.*)\\](\\s*)?$");
    int optIdx = optionalsrx.indexIn(arguments);
    QString optionals = optIdx != -1 ? optionalsrx.cap(1) : "";

    QRegExp commaWithWhiteSpaces("(\\s*,\\s*|\\s*$)");
    required << requireds.split(commaWithWhiteSpaces, QString::SkipEmptyParts);
    optional << optionals.split(commaWithWhiteSpaces, QString::SkipEmptyParts);
}

IlwisTypes OperationMetaData::ilwisType() const
{
    return itSINGLEOPERATION;
}

std::vector<SPOperationParameter> OperationMetaData::getInputParameters() const
{
    return _inputParameters;
}

std::vector<SPOperationParameter> OperationMetaData::getOutputParameters() const
{
    return _outputParameters;
}

quint32 OperationMetaData::inputParameterCount() const
{
    return _inputParameters.size();
}

quint32 OperationMetaData::outputParameterCount() const
{
    return _outputParameters.size();
}

QString OperationMetaData::getNamespace() const
{
    return resource()["namespace"].toString();
}

QString OperationMetaData::getLongName() const
{
    return resource()["longname"].toString();
}

QStringList OperationMetaData::getKeywords() const
{
    QRegExp commaWithWhiteSpaces("(\\s*,\\s*)");
    QString keywords = resource()["keywords"].toString();
    return keywords.split(commaWithWhiteSpaces, QString::SkipEmptyParts);
}

void OperationMetaData::setNamespace(const QString &nspace)
{
    connector()->addProperty("namespace", nspace);
    mastercatalog()->updateItems({resource()});
}

void OperationMetaData::setLongName(const QString &longName)
{
    connector()->addProperty("longname", longName);
    mastercatalog()->updateItems({resource()});
}

void OperationMetaData::setKeywords(const QStringList &keywords)
{
    connector()->addProperty("keywords", keywords);
    mastercatalog()->updateItems({resource()});
}

SPOperationParameter OperationMetaData::inputParameter(int index) const
{
    if ( index < inputParameterCount()){
        return _inputParameters[index];
    }
    return SPOperationParameter();
}

SPOperationParameter OperationMetaData::outputParameter(int index) const
{
    if ( index < outputParameterCount()){
        return _outputParameters[index];
    }
    return SPOperationParameter();
}

void OperationMetaData::clearInputParameters()
{
    removeParameterProperties("pin_%1_", _outputParameters.size());
    mastercatalog()->updateItems({resource()});
    _inputParameters.clear();
}

void OperationMetaData::clearOutputParameters()
{
    removeParameterProperties("pout_%1_", _outputParameters.size());
    mastercatalog()->updateItems({resource()});
    _outputParameters.clear();
}

void OperationMetaData::removeParameterProperties(const QString &base, quint16 size)
{
    for (int i = 1 ; i <= size ; i++) { // correct index here
        connector()->removeProperty(base.arg(QString::number(i)) + "term");
        connector()->removeProperty(base.arg(QString::number(i)) + "name");
        connector()->removeProperty(base.arg(QString::number(i)) + "type");
        connector()->removeProperty(base.arg(QString::number(i)) + "desc");
        connector()->removeProperty(base.arg(QString::number(i)) + "optional");
    }
}

SPOperationParameter OperationMetaData::newParameter(OperationParameter::ParameterKind kind, const QString &term, const QString &name,
                                                     IlwisTypes type, const QString &domain,
                                                     const QString &description, bool optional,
                                                     bool needsQuotes, int linkedinput)
{
    return SPOperationParameter(new OperationParameter(kind, term, name, type, domain, description, optional,needsQuotes, linkedinput));
}

SPOperationParameter OperationMetaData::addParameter(SPOperationParameter parameter)
{
    bool input = parameter->kind() == OperationParameter::ptINPUT;
    if (input) {
        _inputParameters.push_back(parameter);
    } else {
        _outputParameters.push_back(parameter);
    }
    return parameter;
}

QStringList OperationMetaData::choiceList(quint32 paramIndex) const
{
    QString expr = resource()["syntax"].toString();
    int index = expr.indexOf("(");
    QString params = expr.mid(index + 1);
    QStringList parmList = params.split(",");
    if ( paramIndex < parmList.size()){
        QString parm = parmList[paramIndex];
        parm.remove("!");
        QStringList cleanParts = parm.split("=");
        int idx =  cleanParts.size() == 1 ? 0 : 1;
        QStringList choices = cleanParts[idx].split("|");
        if ( choices.size() > 1){
            return choices;
        }

    }
    return QStringList();
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
OperationParameter::OperationParameter(const OperationParameter &operationParameter):
    Identity(name()),
    _term(operationParameter._term),
    _kind(operationParameter._kind),
    _type(operationParameter._type),
    _domainName(operationParameter._domainName),
    _optional(operationParameter._optional),
   _needsQuotes(operationParameter._needsQuotes)
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

QString OperationParameter::term() const
{
    return _term;
}

QString OperationParameter::domainName() const
{
    return _domainName;
}

int OperationParameter::linkedInput() const
{
    return _linkedInput;
}

bool OperationParameter::needsQuotes() const
{
    return _needsQuotes;
}

bool OperationParameter::isOptional() const
{
    return _optional;
}

void OperationParameter::optional(bool optional)
{
    _optional = optional;
}

void OperationParameter::copyMetaToResourceOf(QScopedPointer<ConnectorInterface> &otherconnector, quint16 index) const
{
    QString prefix;
    bool input = kind() == OperationParameter::ptINPUT;
    if (input) {
        prefix = "pin_" + QString::number(index + 1) + "_";
    } else {
        prefix = "pout_" + QString::number(index + 1) + "_";
    }
    otherconnector->addProperty(prefix + "type", _type);
    otherconnector->addProperty(prefix + "term", _term);
    otherconnector->addProperty(prefix + "name", name());
    otherconnector->addProperty(prefix + "desc", description());
    otherconnector->addProperty(prefix + "optional", _optional);
    otherconnector->addProperty(prefix + "needsquotes", needsQuotes());
}

void OperationParameter::prepare(quint64 base)
{
    Identity::prepare(0);
}

OperationParameter::OperationParameter(OperationParameter::ParameterKind kind, const QString &term, const QString &name, IlwisTypes type, const QString &domain, const QString &description, bool optional, bool needsQuotes, int linkedinput) :
    Identity(name),
    _kind(kind),
    _term(term),
    _type(type),
    _domainName(domain),
    _optional(optional),
    _needsQuotes(needsQuotes),
    _linkedInput(linkedinput)
{
    setDescription(description);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
OperationResource::OperationResource()
{

}

OperationResource::OperationResource(const Resource &resource) : Resource(resource)
{

}

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
    //QString urls = url().toString() + "=" + QString::number(id());
    //setUrl(urls);
    addProperty("namespace",nmspace);
}

OperationResource::OperationResource(const QUrl &urlNorm, const QUrl &urlRaw, const QString &nmspace) : Resource(urlNorm,urlRaw,itSINGLEOPERATION)
{
    prepare();
    int index = urlRaw.toString().lastIndexOf("/");
    QString rcontainer = urlRaw.toString().left(index);
    _rawContainer = rcontainer;
    addProperty("namespace",nmspace);
}

void OperationResource::setLongName(const QString &longname)
{
    addProperty("longname",longname);
}

void OperationResource::setSyntax(const QString &syntax)
{
    addProperty("syntax",syntax);
    setInParameterNameFromSyntax(syntax);
}

void OperationResource::setInParameterNameFromSyntax(const QString &syntax)
{
    QString e = syntax;
    int index = e.indexOf('(');
    int len = e.size() - index - 2;
    int prev = 0;
    int i = 1;
    int paramIndex = 1;
    int trimEnd = 1;
    QString rest = e.mid(index + 1 , len);
    foreach(const QChar& cu, rest) {
        char c = cu.toLatin1();
        if (c== '[')
            trimEnd++;

        if (c == ',') {
            QString value = rest.mid(prev, i - prev - trimEnd);
            value = value.trimmed();
            int index2 = value.indexOf('=');
            if (index2 >= 0)
                value = value.left(index2); // If it has an if, take everything from the left.
            addProperty("pin_" + QString::number(paramIndex) + "_parm_name",value);
            ++paramIndex;
            prev = i;
        }
        ++i;
    }
    QString value = rest.mid(prev, i - prev - trimEnd);
    value = value.trimmed();
    int index2 = value.indexOf('=');
    if (index2 >= 0)
        value = value.left(index2);
    addProperty("pin_" + QString::number(paramIndex) + "_parm_name",value);
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

void OperationResource::addInParameter(quint32 order, IlwisTypes type, const QString &nme, const QString &description, UIElement altUIType, bool needsQuotes)
{
    QString prefix = "pin_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", nme);
    addProperty(prefix + "desc", description);
    addProperty(prefix + "optional", false);
    addProperty(prefix + "needsquotes", needsQuotes);
    if ( altUIType != ueNONE)
        addProperty(prefix + "altUIType", altUIType);
}

void OperationResource::parameterNeedsQuotes(quint32 order)
{

    QString prefix = "pin_" + QString::number(order + 1) + "_";
    addProperty(prefix + "needsquotes", true);
}

void OperationResource::setOutputIsInput(int outParm, int inParm)
{
    QString prefix = "pout_" + QString::number(outParm + 1) + "_";
    addProperty(prefix + "input", inParm);
}

bool OperationResource::compatibleOperationAlreadyExists(bool ignoreSelf)
{
    std::vector<Resource> items = mastercatalog()->select("resource='" + url().toString() + "'");
    if ( items.size() == 0)
        return false;
    if (!ignoreSelf){
       for(const Resource& item : items){
           if ( item.id() == id())
               return true;
       }
    }
    int pinCountOther = (*this)["inparameters"].toList().size();
    for(const Resource& item : items){
        int pinCountSelf= item["inparameters"].toList().size();
        if ( pinCountSelf == pinCountOther){
             for(int order = 1; order < pinCountSelf; ++order){
                QString prefix = "pin_" + QString::number(order) + "_";
                IlwisTypes inType = item[prefix + "type"].toULongLong();
                IlwisTypes outType =  (*this)[prefix + "type"].toULongLong();
                if ( !hasType(inType,outType))
                    return false;
             }
        }
    }
    return true;
}

void OperationResource::addOptionalInParameter(quint32 order, IlwisTypes type, const QString &name, const QString &description, UIElement altUIType)
{
    addInParameter(order, type, name, description, altUIType);
    QString prefix = "pin_" + QString::number(order + 1) + "_";
    addProperty(prefix + "optional", true);
}

void OperationResource::addOutParameter(quint32 order, IlwisTypes type, const QString &nme, const QString &description)
{
    QString prefix = "pout_" + QString::number(order + 1) + "_";
    addProperty(prefix + "type", type);
    addProperty(prefix + "name", nme);
    addProperty(prefix + "desc", description);
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
    QStringList parts = words.split(",");
    QString result;
    for(auto part : parts){
        if ( result != "")
            result += ",";
        result += part.trimmed();
    }
    addProperty("keyword", result.toLower());
}

void OperationResource::addValidation(quint32 parent, quint32 child, const QString &validationCondition)
{
    QString prefix = "pin_" + QString::number(child + 1) + "_";
    addProperty(prefix + "validationsource", parent);
    addProperty(prefix + "validationcondition", validationCondition);
}



