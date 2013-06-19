#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Kernel_global.h"
#include <QVariant>
#include <QMultiHash>

#define ANONPREFIX "ANON"

namespace Ilwis {
class KERNELSHARED_EXPORT Symbol{
public:
    Symbol(int scope=iUNDEF, quint64 tp=itUNKNOWN, const QVariant& v=QVariant()) ;
    quint64 _type;
    int _scope;
    QVariant _var;
    QVariant _modifier;
    bool isValid() const;
};

class KERNELSHARED_EXPORT SymbolTable //: private QHash<QString, Symbol>
{
public:
    SymbolTable();

    void addSymbol(const QString& name, int scope, quint64 tp, const QVariant &v=QVariant());
    QVariant getValue(const QString& name, int scope=1000) const;
    Symbol getSymbol(const QString& name, int scope=1000) const;

    static bool isNumerical(const QVariant &var) ;
    static bool isRealNumerical(const QVariant &var) ;
    static bool isIntegerNumerical(const QVariant &var) ;
    static bool isDataLink(const QVariant &value);
    static QString newAnonym();
private:
    QHash<QString, Symbol> _symbols;
    static quint64 _symbolid;

    
    
};
}

#endif // SYMBOLTABLE_H
