#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Kernel_global.h"
#include <QVariant>
#include <QMultiHash>

namespace Ilwis {
struct KERNELSHARED_EXPORT Symbol{
    Symbol(int scope, int tp, const QVariant& v) : _type(tp), _scope(scope), _var(v) {}
    int _type;
    int _scope;
    QVariant _var;
    QVariant _modifier;
};

class KERNELSHARED_EXPORT SymbolTable //: private QHash<QString, Symbol>
{
public:
    SymbolTable();

    void addSymbol(const QString& name, int scope, quint64 tp, const QVariant &v=QVariant());
    QVariant get(const QString& name, int scope, bool& ok);

    static bool isNumerical(const QVariant &var) ;
    static bool isRealNumerical(const QVariant &var) ;
    static bool isIntegerNumerical(const QVariant &var) ;
    static bool isDataLink(const QVariant &value);
private:
    QMultiHash<QString, Symbol> _symbols;

    
    
};
}

#endif // SYMBOLTABLE_H
