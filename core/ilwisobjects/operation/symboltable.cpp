#include <QString>
#include <QFileInfo>
#include <QVector>
#include <QUrl>
#include <QRegExp>
#include <QStringList>
#include "kernel.h"
#include "raster.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "symboltable.h"

using namespace Ilwis;

quint64 SymbolTable::_symbolid = 0;

SymbolTable::SymbolTable() //:
    //QHash<QString, Symbol>()
{
}

SymbolTable::~SymbolTable(){
    _symbols.clear();
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
            if ( typname == "Coordinate")
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
            QString tp = iter.value()._var.typeName();
            if ( tp == "QVariantList"){
                QVariantList lst = iter.value()._var.value<QVariantList>();
                return lst[0];
            }
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
            bool isAnonymous = name.indexOf(ANONYMOUS_PREFIX) == 0;
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

void SymbolTable::unloadRasters()
{
    for(Symbol& sym: _symbols) {
        if ( sym._type == itRASTER) {
            IRasterCoverage raster = sym._var.value<IRasterCoverage>();
            if ( raster.isValid())
                raster->unloadBinary();
        }
    }
}

IlwisTypes SymbolTable::ilwisType(const QVariant &value, const QString& symname) const
{
    if ( symname == sUNDEF) { // not a referenced value
        return Domain::ilwType(value);
    }

    auto iter = _symbols.find(symname);
    if ( iter != _symbols.end()) {
        return iter.value()._type;
    }

    IlwisTypes tp = IlwisObject::findType(symname);
    return tp;
}

bool SymbolTable::isNumerical(const QVariant& var) {
    return isRealNumerical(var) || isIntegerNumerical(var);
}

bool SymbolTable::isString(const QVariant& var) {
    return var.canConvert<QString>();
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
        bool ok;
        resolvedv.toDouble(&ok);
        if (ok)
            return false;
        resolvedv = Ilwis::context()->workingCatalog()->resolve(resolvedv);
        if ( resolvedv == sUNDEF) {
            quint64 id = mastercatalog()->name2id(resolvedv);
            if ( id != i64UNDEF)
                return true;
            return false;
        }
    }
    return true;

}

QString SymbolTable::newAnonym()
{
    _symbolid++;
    return QString("%1%2").arg(ANONYMOUS_PREFIX).arg(_symbolid);
}


Symbol::Symbol(int scope, quint64 tp, const QVariant &v) : _type(tp), _scope(scope), _var(v)
{
}

Symbol::~Symbol()
{

}

bool Symbol::isValid() const
{
    return _var.isValid() && _scope != iUNDEF;
}


