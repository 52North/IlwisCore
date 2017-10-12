#include <QString>
#include <QFileInfo>
#include <QVector>
#include <QUrl>
#include <QRegExp>
#include <QStringList>
#include "kernel.h"
#include "raster.h"
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

void SymbolTable::setSymbol(const QString &name, const Symbol& sym)
{
    _symbols[name] = sym;
}

QVariant SymbolTable::getValue(const QString &name, int scope) const
{
    if ( name.isNull() || name.isEmpty())
        return QVariant();

    QHash<QString, Symbol>::const_iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        QString tp = iter.value()._var.typeName();
        if ( tp == "QVariantList"){
            QVariantList lst = iter.value()._var.value<QVariantList>();
            return lst[0];
        }
        return iter.value()._var;
        ++iter;
    }
    return QVariant();
}

Symbol SymbolTable::getSymbol(const QString &name, GetAction act, int scope)
{
    if ( name.isNull() || name.isEmpty())
        return Symbol();

    QHash<QString, Symbol>::iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        Symbol sym = iter.value();
        bool isAnonymous = name.indexOf(ANONYMOUS_PREFIX) == 0;
        if ((isAnonymous && act == gaREMOVEIFANON) || act == gaREMOVE)
            _symbols.erase(iter);
        return sym;
        ++iter;
    }
    return Symbol();
}

Symbol SymbolTable::getSymbol(const QString &name, int scope) const
{
    QHash<QString, Symbol>::const_iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
            return iter.value();
    }
    return Symbol();
}

void SymbolTable::removeAllBut(std::vector<QString> &exceptions)
{
    std::map<QString, bool> remove;
    for(auto symIter= _symbols.begin(); symIter != _symbols.end(); ++symIter)    {
        auto iter = std::find(exceptions.begin(), exceptions.end(), symIter.key());
        remove[symIter.key()] = iter == exceptions.end();
    }
    for(auto removePair : remove){
        if ( removePair.second){
            auto iter = _symbols.find(removePair.first);
            _symbols.erase(iter);
        }
    }
}

const QHash<QString, Symbol> SymbolTable::symbols() const
{
    return _symbols;
}

void SymbolTable::unloadData()
{
    for(Symbol& sym: _symbols) {
        if ( sym._type == itRASTER) {
            IRasterCoverage raster = sym._var.value<IRasterCoverage>();
            if ( raster.isValid())
                raster->unload();
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
    if ( tp != itUNKNOWN)
        return tp;
    quint64 id = mastercatalog()->name2id(symname);
    if ( id != i64UNDEF){
        IIlwisObject obj = mastercatalog()->get(id);
        if ( obj.isValid()){
            return obj->ilwisType();
        }
    }
    return itUNKNOWN;
}

void SymbolTable::copyFrom(ExecutionContext *ctx, const SymbolTable &symTable)
{
    for(const QString& result : ctx->_results){
        Symbol symbol = symTable.getSymbol(result);
        if ( symbol.isValid()){
            _symbols[result] = symbol;
        }
    }
}

bool SymbolTable::isNumerical(const QVariant& var) {
    return isRealNumerical(var) || isIntegerNumerical(var);
}

bool SymbolTable::isString(const QVariant& var) {
    return var.canConvert<QString>();
}

bool SymbolTable::isRealNumerical(const QVariant& var)  {
    bool ok =  var.type() == QMetaType::Float ||  var.type() == QMetaType::Double;
    if ( ok)
        return true;
    var.toDouble(&ok);
    return ok;
}

bool SymbolTable::isIndex(int index, const QVariantList& var) {
  QString tpname = var[index].typeName();
  if ( tpname == "std::vector<quint32>" || tpname == "Indices")
      return true;

  return false;
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


