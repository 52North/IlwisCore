#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "kernel_global.h"
#include <QVariant>
#include <QMultiHash>

namespace Ilwis {
class KERNELSHARED_EXPORT Symbol{
public:
    Symbol(int scope=iUNDEF, quint64 tp=itUNKNOWN, const QVariant& v=QVariant()) ;
    virtual ~Symbol();
    quint64 _type;
    int _scope;
    QVariant _var;
    QVariant _modifier;
    bool isValid() const;
};

class KERNELSHARED_EXPORT SymbolTable //: private QHash<QString, Symbol>
{
public:
    enum GetAction { gaKEEP, gaREMOVE, gaREMOVEIFANON};
    SymbolTable();
    virtual ~SymbolTable();

    void addSymbol(const QString& name, int scope, quint64 tp, const QVariant &v=QVariant());
    QVariant getValue(const QString& name, int scope=1000) const;
    Symbol getSymbol(const QString& name, GetAction act=gaKEEP, int scope=1000);
    Symbol getSymbol(const QString& name, int scope=1000) const;
    template<typename T> T getValue(const QString& name){
        QVariant var = getValue(name)    ;
        return var.value<T>();
    }

    void unloadRasters();
    IlwisTypes ilwisType(const QVariant &value, const QString &symname) const;

    static bool isNumerical(const QVariant &var) ;
    static bool isRealNumerical(const QVariant &var) ;
    static bool isIntegerNumerical(const QVariant &var) ;
    static bool isDataLink(const QVariant &value);
    static QString newAnonym();
    static bool isString(const QVariant &var);
    static bool isIndex(int index, const QVariantList &var);
private:
    QHash<QString, Symbol> _symbols;
    static quint64 _symbolid;

    
    
};
}

typedef std::vector<quint32> Indices;
typedef std::vector<double> Factors;

Q_DECLARE_METATYPE(Indices)
Q_DECLARE_METATYPE(Factors)

#endif // SYMBOLTABLE_H
