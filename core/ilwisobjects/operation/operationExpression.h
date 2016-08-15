#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QList>
#include <QMap>
#include "kernel_global.h"

namespace Ilwis {
    class SymbolTable;

    enum OperationType{otUnknown=0,otFunction=1, otCommand=2, otSelection=4};

class KERNELSHARED_EXPORT Parameter : public Identity{
public:

    enum PathType{ptNONE, ptLOCALOBJECT, ptREMOTE, ptUNDEFINED, ptIRRELEVANT};

    Parameter();
    Parameter(const QString& name, const QString& value, IlwisTypes ,const SymbolTable& );
    Parameter(const QString& value, IlwisTypes, const SymbolTable &);
    virtual ~Parameter();
    QString value() const;
    QString domain() const;
    void domain(const QString& dom);
    IlwisTypes valuetype() const;
    bool isEqual(const Parameter& parm) const;
    bool isValid() const;
    Parameter::PathType pathType() const;
    static IlwisTypes determineType(const QString &value, const SymbolTable& symtab) ;


private:
    QString _key;
    QString _value;
    IlwisTypes _type;
    QString _domain;
};

/*!
 The OperationExpression class a parser for fully defined expressions. This class will parse expressions of the type function and command and split it into its parts. The assumption is that the expression is already parsed to a level that all parts can not be parsed further.
 more complex expressions need first go to the calculator to be reduced to this level. Note that functions have a fixed order of parameters v = func(a,b) is different from b fun(b,a)
 */
class KERNELSHARED_EXPORT OperationExpression
{
public:
    /*!
     *  OperationExpression empty expression
     */
    OperationExpression();
    ~OperationExpression();
    /*!
     * Constructs an expression object of an textual expression.
     *
     *Expressions have either the form of a function which takes the form of output*={id}(paramteres*) or a command with the form {id} (paramters)*. The class will splt this in
     *ids, input parametrs and return parameters
     *
     * \param expr textual form of the expression
     * \param type enum marking the type of the expression, function or command.
     */
    OperationExpression(const QString& expr, const SymbolTable& symtab=SymbolTable());
    /*!
     *  returns the parameter at a defined placed in either the input or the output
     * \param index rank order number of the parameters to be returned
     * \param in input parameter or output parameter
     * \return a parameter
     */
    Parameter parm(int index, bool in=true) const;
    /*!
     *  returns the parameter at a defined name in either the input or the output
     * \param key name of the parameters to be returned
     * \param toLower name will be in lowercase (or not)
     * \param in input parameter or output parameter
     * \return a parameter
     */

    Parameter parm(const QString key, bool toLower=true, bool in=true) const;
    /*!
     *  name returns the name of the command or function
     * \param toLower toLower name will be in lowercase (or not)
     * \return the function/command id
     */
    QString name(bool toLower=true) const;
    /*!
     *  parameterCount returns the number of input or outparameters
     * \param in input or output
     * \return count
     */
    int parameterCount(bool in=true) const;
    /*!
     *  isValid minimum condition for an expression to be valid, it must have a name
     *
     *  An expression might have no return types and no parameters but it always must have a name
     * \return true if there is a name
     */
    bool inputIsKeyword() const { return _inParametersMap.size() > 0; }
    bool isValid() const;
    bool isRemote() const;
    QUrl metaUrl(bool simple=true) const;
    void setExpression(const QString &e, const SymbolTable &symtab);
    bool matchesParameterCount(const QString &match, bool in=true) const;
    QString toString(bool rightsideonly=false) const;
    QString toPythonExpression() const;
    template<typename T> T input(quint32 ) { return T(); }
    template<typename T> T input(quint32, bool& ok ) { return T(); }
    QMap<QString, Parameter> getKeywordParameters() const {return _inParametersMap; }
    static OperationExpression createExpression(quint64 operationid, const QString &parameters, bool acceptIncompleteExpressions=false);
private:
    QString _name;
    QMap<QString, Parameter> _inParametersMap;
    QList<Parameter> _inParameters;
    QList<Parameter> _outParameters;
    OperationType _type;
    QString _selection;
    bool _isRemote = false;

    void parseFunctionExpression(const QString &txt, const SymbolTable &symtab);
    void parseCommandExpression(const QString &expr, const SymbolTable &symtab);
    void parseSelectors(const QString& selectors, const SymbolTable &symtab);
    void specialExpressions(const QString &e, const SymbolTable &symtab);
    static QString modifyTableOutputUrl(const QString &output, const QStringList &parms);
};

template<> inline int OperationExpression::input<qint32>(quint32 parmIndex, bool& ok) {
    return parm(parmIndex).value().toInt(&ok);
}

template<> inline int OperationExpression::input<qint32>(quint32 parmIndex) {
    return parm(parmIndex).value().toInt();
}

template<> inline quint32 OperationExpression::input<quint32>(quint32 parmIndex, bool& ok) {
    return parm(parmIndex).value().toUInt(&ok);
}

template<> inline quint32 OperationExpression::input<quint32>(quint32 parmIndex) {
    return parm(parmIndex).value().toUInt();
}

template<> inline double OperationExpression::input<double>(quint32 parmIndex, bool& ok) {
    return parm(parmIndex).value().toDouble(&ok);
}

template<> inline double OperationExpression::input<double>(quint32 parmIndex) {
    return parm(parmIndex).value().toDouble();
}



template<> inline QString OperationExpression::input<QString>(quint32 parmIndex) {
    return parm(parmIndex).value().remove('"');
}

template<> inline bool OperationExpression::input<bool>(quint32 parmIndex) {
    QString v = parm(parmIndex).value().toLower();
    return v == "yes" || v == "true";
}

template<> inline bool OperationExpression::input<bool>(quint32 parmIndex, bool& ok) {
    QString v = parm(parmIndex).value().toLower();
    ok = true;

    if (v != "yes" && v != "true" && v != "no" && v != "false")
        ok = false;
    return v == "yes" || v == "true";
}




}

Q_DECLARE_METATYPE(Ilwis::OperationExpression)

#endif // EXPRESSION_H
