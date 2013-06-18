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

SymbolTable::SymbolTable() //:
    //QHash<QString, Symbol>()
{
}

void SymbolTable::addSymbol(const QString &name, int scope, quint64 tp, const QVariant& v)
{
    bool ok;
    get(name,scope, ok); // do we already have it?
    if (ok)
        return;
    if ( tp == 0) {
        if ( isRealNumerical(v))
            tp = itDOUBLE;
        else if (isIntegerNumerical(v))
            tp = itINT32;
        else if ( v.type() == QMetaType::QString)
            tp = itSTRING;

    }
    _symbols.insert(name, Symbol(scope, tp, v));
}

QVariant SymbolTable::get(const QString &name, int scope, bool &ok)
{
    ok = true;
    QHash<QString, Symbol>::iterator   iter = _symbols.find(name);
    while (iter != _symbols.end() && iter.key() == name) {
        if ( iter.value()._scope == scope) {
            return iter.value()._var;
        }
        ++iter;
    }
    ok = false;
    return QVariant();
}

bool SymbolTable::isNumerical(const QVariant& var) {
    return isRealNumerical(var) || isIntegerNumerical(var);
}

bool SymbolTable::isRealNumerical(const QVariant& var)  {
    return var.type() == QMetaType::Float ||  var.type() == QMetaType::Double;
}

bool SymbolTable::isIntegerNumerical(const QVariant& var) {
    QMetaType::Type tp = static_cast<QMetaType::Type>(var.type());
    return tp == QMetaType::Int ||  tp == QMetaType::UInt ||
            tp == QMetaType::LongLong ||  tp == QMetaType::ULongLong ||
            tp == QMetaType::Long ||  tp == QMetaType::ULong ||
            tp == QMetaType::Short || tp == QMetaType::UShort ||
            tp == QMetaType::Char || tp == QMetaType::UChar;
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
