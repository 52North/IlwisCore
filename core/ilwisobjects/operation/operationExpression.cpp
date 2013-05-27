#include <QString>
#include <QUrl>
#include "kernel.h"
#include "ilwisobject.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "OperationExpression.h"

using namespace Ilwis;

Parameter::Parameter() {
}

Parameter::Parameter(const QString &key, const QString &value, IlwisTypes tp) :
    Identity(key),
    _value(value),
    _domain(sUNDEF)
{
    _type = tp;
    if ( _type == itUNKNOWN)
        _type = Parameter::determineType(value);
}

Parameter::Parameter(const QString &rawvalue, IlwisTypes tp) : _domain(sUNDEF)
{
    QString value = rawvalue;
    int index = value.indexOf('[');
    if ( index != -1) {
        int index2 = value.indexOf(']');
        QString rangePart = value.mid(index + 1,  index2 - index - 1);
        _modifiers["range"] = rangePart;
        value = value.left(index);
    }
    _value = value;
    _type = tp;
    if ( _type == itUNKNOWN)
        _type = Parameter::determineType(_value);
}

Ilwis::Parameter::~Parameter()
{
}

QString Parameter::value() const
{
    return _value;
}

QString Parameter::domain() const
{
    return _domain;
}

void Parameter::domain(const QString &dom)
{
    _domain = dom;
}

QString Parameter::modifier(const QString &key) const
{
    auto iter = _modifiers.find(key);
    if ( iter != _modifiers.end())
        return (*iter).second;
    return sUNDEF;
}

IlwisTypes Parameter::valuetype() const
{
    return _type;
}

bool Parameter::isEqual(const Parameter &parm) const
{
    return parm.name() == name() && parm.value() == _value;
}

bool Parameter::isValid() const
{
    return !_value.isEmpty();
}

IlwisTypes Parameter::determineType(const QString& value) {
    IlwisTypes tp = IlwisObject::findType(value);
    if ( tp != itUNKNOWN)
        return tp;

    QString s = context()->workingCatalog()->resolve(value);
    IlwisTypes type = IlwisObject::findType(s) ;
    if ( type != itUNKNOWN)
        return type;

    if ( value.left(10) == INTERNAL_PREFIX) {
        QString sid = value.mid(11);
        bool ok;
        quint64 id = sid.toLongLong(&ok);
        if ( ok) {
            ESPObject obj =  mastercatalog()->get(id);
            if ( obj.data() != 0)
                return obj->ilwisType();
        }

    }


    bool ok;
    ushort vu = value.toUShort(&ok);
    if ( ok && vu < 255)
        return itUINT8;
    if ( ok)
        return itUINT16;
    short vs = value.toShort(&ok);
    if ( ok && vs > -128 && vs < 128)
        return itINT8;
    if ( ok)
       return itINT16;
    value.toULong(&ok);
    if ( ok)
        return itUINT32;
    value.toLong(&ok);
    if ( ok)
       return itINT32;
    value.toLongLong(&ok);
    if ( ok)
        return itINT64;
    value.toDouble(&ok);
    if ( ok)
       return itDOUBLE;

    return itSTRING;

}

//----------------------------------------

OperationExpression::OperationExpression()
{
}

Ilwis::OperationExpression::~OperationExpression()
{
}

OperationExpression::OperationExpression(const QString &e)
{
    setExpression(e);
}

void OperationExpression::setExpression(const QString &e) {
    _name = "";
    _inParameters.clear();
    _outParameters.clear();
    _type = otFunction;
    QString dummy = e;
    int index1 = e.indexOf(" ");
    int index2 = e.indexOf("=");
    if ( index1 != -1 && ( index1 < index2 || index2 == -1)) {
        _type = otCommand;
        for(int i = 0; i < index1; ++i){
            if ( !(e[i].isDigit() || e[i].isLetter() || e[i] == '-')){
                _type = otFunction;
                break;
            }
        }
    }

    if ( _type == otFunction) {
        parseFunctionExpression(e);
    } else if ( _type == otCommand) {
        parseCommandExpression(e);
    }
}

void OperationExpression::parseCommandExpression(const QString &e) {
    int blockCount = 0;
    int quoteCount = 0;
    int count = 0;
    QList<int> indexes;
    foreach(const QChar& cu, e) {
        char c = cu.toLatin1(); // eessions are not internatiolized, so its allowed
        if ( c == '(' && quoteCount == 0)
            blockCount++;
        if ( c == ')' && quoteCount == 0)
            blockCount--;
        if ( c == '"' && quoteCount == 0)
            quoteCount++;
        else if ( c == '"' && quoteCount != 0)
            quoteCount--;

        if ( c == ' ' && blockCount == 0 && quoteCount == 0)
            indexes.push_back(count++);
        ++count;
    }
    indexes.push_back(count - 1);
    int current = 0;
    foreach(int index, indexes) {
        QString part = e.mid(current, index - current) ;
        part = part.trimmed();
        if ( current == 0)
            _name = part;
        else
            _inParameters.push_back(Parameter(part));
        current = index;
    }
}

void OperationExpression::parseFunctionExpression(const QString &e) {
    int index = e.indexOf("(");
    if ( index == -1 )
        return;
    int index2 = e.indexOf("=");
    if ( index2 != -1)
        _name = e.mid(index2+1, index - index2 - 1);
    else
        _name = e.left(index);
    int len =  e.size() - index - 2;
    QString rest = e.mid(index + 1 , len);
    QString start = e.left(index2);
    int blockCount = 0;
    int quoteCount = 0;
    int count = 0;
    int cur = 0;
    std::vector<int> indexes;
    if ( index2 != -1) {
        foreach(const QChar& cu, start) {
            char c = cu.toLatin1();
            if ( c == '{' && quoteCount == 0){
                blockCount++;
            }
            if ( c == '}' && quoteCount == 0){
                blockCount--;
            }

            if ( c == ',' || blockCount == 1)
                indexes.push_back(count++);
            ++count;
        }
        qint32 shift = 0;
        indexes.push_back(start.size());
        for(int i =0; i < indexes.size(); ++i) {
            int index3 = indexes[i];
            QString part = start.mid(cur, index3 - cur - shift) ;
            part = part.trimmed();
            _outParameters.push_back(Parameter(part));
            shift = 1;
            cur = index3 + shift;
        }
    }

    //indexes.push_back(0);
    indexes.clear();
    blockCount = quoteCount = count = 0;
    foreach(const QChar& cu, rest) {
        char c = cu.toLatin1(); // eessions are not internatiolized, so its allowed
        if ( c == '(' && quoteCount == 0)
            blockCount++;
        if ( c == ')' && quoteCount == 0)
            blockCount--;
        if ( c == '"' && quoteCount == 0)
            quoteCount++;
        else if ( c == '"' && quoteCount != 0)
            quoteCount--;

        if ( c == ',' && blockCount == 0 && quoteCount == 0)
            indexes.push_back(++count);
        else
            ++count;
    }
    cur = 0;
    indexes.push_back(rest.size() + 1);
    for(int i =0; i < indexes.size(); ++i) {
        int index4 = indexes[i];
        QString part = rest.mid(cur, index4 - cur - 1) ;
        part = part.trimmed();
        _inParameters.push_back(Parameter(part));
        cur = index4;
    }
}

Parameter OperationExpression::parm(int index, bool in) const
{
    const QList<Parameter>& parameters = in ? _inParameters : _outParameters;
    if ( index < parameters.size())
        return parameters[index];

    return Parameter();
}

Parameter OperationExpression::parm(const QString searchValue, bool caseInsensitive, bool in) const
{
    const QList<Parameter>& parameters = in ? _inParameters : _outParameters;
    if ( !searchValue.isEmpty() ) {
        foreach(const Parameter& parm, parameters) {
            QString k2 = _type == otFunction ? parm.name(): parm.value();
            if ( caseInsensitive){
                QString k1 = searchValue.toLower();
                k2 = k2.toLower();
                if ( k1 == k2)
                    return parm;
            } else {
                if ( k2 == searchValue)
                return parm;
            }
        }
    }
    return Parameter();
}

QString OperationExpression::name(bool caseInsensitive) const
{
    if ( caseInsensitive)
        return _name.toLower();
    return  _name;
}

bool OperationExpression::matchesParameterCount(const QString& match, bool in) const {
    int count = parameterCount(in);
    QStringList parts = match.split("|");
    foreach(const QString& part, parts) {
        bool ok;
        int index = part.toInt(&ok);
        if (!ok) {
            return ERROR0("Illegal metdata definition");
        }
        if ( index == count)
            return true;
    }
    return false;


}

int OperationExpression::parameterCount(bool in) const
{
    const QList<Parameter>& parameters = in ? _inParameters : _outParameters;

    return parameters.size();
}

bool OperationExpression::isValid() const
{
    return !_name.isEmpty();
}

QUrl Ilwis::OperationExpression::metaUrl(bool simple) const
{
    QString url = "ilwis://operations/" + _name;
    if ( !simple) {
        for(int i=0; i < _inParameters.size(); ++i) {
            if ( i == 0)
                url += "?";
            else
                url += "&";

            url += QString("pin::%1::type=%2").arg(i).arg(_outParameters[i].valuetype());
        }
        for(int i=0; i < _outParameters.size(); ++i) {
            if ( i == 0)
                url += "?";
            else
                url += "&";

            url += QString("pout::%1::type=%2").arg(i).arg(_outParameters[i].valuetype());
        }
    }
    return QUrl(url);

}






