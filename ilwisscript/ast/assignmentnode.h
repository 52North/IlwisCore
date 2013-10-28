#ifndef AssignmentNode_H
#define AssignmentNode_H

#include "ilwisdata.h"

namespace Ilwis {

class Selector;

class AssignmentNode : public ASTNode
{
public:
    AssignmentNode();
    bool isDefinition() const;
    void setDefintion(bool yesno);
    void setExpression(ExpressionNode *node);
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void addOutputs(OutParametersNode *p);

private:
    template<typename T1> bool copyObject(const Symbol& sym, const QString& name,SymbolTable &symbols, bool useMerge=false) {
        IlwisData<T1> source =  sym._var.value<IlwisData<T1>>();
        if (!source.isValid())
            return false;
        bool wasAnonymous = source->isAnonymous();
        bool done = false;
        IlwisData<T1> target;
        if ( useMerge) {
            if ( target.prepare(name)) {
                done = target->merge(source.ptr());
            }
        }
        if(!done) {
            T1 *obj = static_cast<T1 *>(source->copy());
            if(!obj)
                return false;
            obj->setName(name);
            target.set(obj);
        }
        if ( !target.isValid())
            return false;

        QVariant var;
        var.setValue<IlwisData<T1>>(target);
        symbols.addSymbol(name, 1000, target->ilwisType(), var);
        if ( wasAnonymous)
            mastercatalog()->addItems({target->source(IlwisObject::cmINPUT | IlwisObject::cmEXTENDED)});

        return true;
    }
    IIlwisObject getObject(const Symbol &sym) const;
    void getFormat(QSharedPointer<Ilwis::ASTNode> &node, QString &format, QString &fnamespace) const;
    void store2Format(QSharedPointer<Ilwis::ASTNode> &node, const Symbol &sym, const QString &result);
    bool _defintion;
    QSharedPointer<ExpressionNode> _expression;
    QSharedPointer<OutParametersNode> _outParms;
};
}

#endif // AssignmentNode_H
