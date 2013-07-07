#include <QString>
#include <QFileInfo>
#include <QVector>
#include <QUrl>
#include <QRegExp>
#include "ilwis.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "symboltable.h"

using namespace Ilwis;

quint64 SymbolTable::_symbolid = 0;

SymbolTable::SymbolTable() //:
    //QHash<QString, Symbol>()
{
}

void SymbolTable::addSymbol(const QString &name, int scope, quint64 tp, const QVariant& v)
{
    QVariant var = getValue(name,scope); // do we already have it?
    if (var.isValid()){
        _symbols[name]._var = v;
        _symbols[name]._type = tp;
        return;
    }
    if ( tp == 0) {
        if ( isRealNumerical(v))
            tp = itDOUBLE;
        else if (isIntegerNumerical(v))
            tp = itINT32;
        else {
            if ( v.type() == QMetaType::QString)
                tp = itSTRING;
            QString typname = v.typeName();
            if ( typname == "Ilwis::Coordinate")
                tp = itCOORDINATE;
        }

    }
    Symbol sym(scope, tp, v);
    _symbols[name] = sym;
}

QVariant SymbolTable::getValue(const QString &name, int scope) const
{
    QHash<QString, Symbol>::const_iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        if ( iter.value()._scope == scope) {
            return iter.value()._var;
        }
        ++iter;
    }
    return QVariant();
}

Symbol SymbolTable::getSymbol(const QString &name, GetAction act, int scope)
{
    QHash<QString, Symbol>::iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        if ( iter.value()._scope <= scope) {
            Symbol sym = iter.value();
            bool isAnonymous = name.indexOf(INTERNAL_PREFIX) == 0;
            if ((isAnonymous && act == gaREMOVEIFANON) || act == gaREMOVE)
                _symbols.erase(iter);
            return sym;
        }
        ++iter;
    }
    return Symbol();
}

Symbol SymbolTable::getSymbol(const QString &name, int scope) const
{
    QHash<QString, Symbol>::const_iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        if ( iter.value()._scope == scope) {
            return iter.value();
        }
        ++iter;
    }
    return Symbol();
}

bool SymbolTable::isNumerical(const QVariant& var) {
    return isRealNumerical(var) || isIntegerNumerical(var);
}

bool SymbolTable::isRealNumerical(const QVariant& var)  {
    QString tpname = var.typeName();
    bool ok =  var.type() == QMetaType::Float ||  var.type() == QMetaType::Double;
    if ( ok)
        return true;
    var.toDouble(&ok);
    return ok;
}

bool SymbolTable::isIntegerNumerical(const QVariant& var) {
    QMetaType::Type tp = static_cast<QMetaType::Type>(var.type());
    bool ok = tp == QMetaType::Int ||  tp == QMetaType::UInt ||
            tp == QMetaType::LongLong ||  tp == QMetaType::ULongLong ||
            tp == QMetaType::Long ||  tp == QMetaType::ULong ||
            tp == QMetaType::Short || tp == QMetaType::UShort ||
            tp == QMetaType::Char || tp == QMetaType::UChar;
    if ( ok)
        return true;
    var.toLongLong(&ok);
    if ( ok)
        return true;
    var.toULongLong(&ok);
    return ok;
}

bool SymbolTable::isDataLink(const QVariant& value) {
    QString resolvedv = value.toString();
    if ( !resolvedv.contains(QRegExp("\\\\|/"))) {
        resolvedv = Ilwis::context()->workingCatalog()->resolve(resolvedv);
        if ( resolvedv == sUNDEF)
            return false;
    }
    return true;

}

QString SymbolTable::newAnonym()
{
    _symbolid++;
    return QString("%1%2").arg(INTERNAL_PREFIX).arg(_symbolid);
}


Symbol::Symbol(int scope, quint64 tp, const QVariant &v) : _type(tp), _scope(scope), _var(v)
{
}

bool Symbol::isValid() const
{
    return _var.isValid() && _scope != iUNDEF;
}


