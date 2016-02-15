#ifndef AssignmentNode_H
#define AssignmentNode_H

#include "ilwisdata.h"

namespace Ilwis {

class Selector;
class SelectNode;

class AssignmentNode : public ASTNode
{
public:
    AssignmentNode();
    bool isDefinition() const;
    void setDefintion(bool yesno);
    void setExpression(ASTNode *node);
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void addOutputs(OutParametersNode *p);
    void setOutId(IDNode *idnode);
private:
    template<typename T1> bool copyObject(const Symbol& sym, const QString& name,SymbolTable &symbols, bool useMerge=false) {
        IlwisData<T1> source =  sym._var.value<IlwisData<T1>>();
        if (!source.isValid())
            return false;
        bool wasAnonymous = source->isAnonymous();
        bool done = false;
        IlwisData<T1> target;
        //target.prepare(name, source->ilwisType());
        if ( useMerge) {
            if ( target.prepare(name, source->ilwisType())) {
                done = target->merge(source.ptr());
            }
        }
        if(!done) {
            T1 *obj = static_cast<T1 *>(source->clone());
            if(!obj)
                return false;
            obj->name(name);
            target.set(obj);
        }
        if ( !target.isValid())
            return false;

        QVariant var;
        var.setValue<IlwisData<T1>>(target);
        symbols.addSymbol(name, 1000, target->ilwisType(), var);
        if ( wasAnonymous)
            mastercatalog()->addItems({target->resource(IlwisObject::cmINPUT | IlwisObject::cmEXTENDED)});

        return true;
    }
    IIlwisObject getObject(const Symbol &sym) const;
    void getFormat(QSharedPointer<Ilwis::ASTNode> &node, QString &format, QString &fnamespace) const;
    void store2Format(QSharedPointer<Ilwis::ASTNode> &node, const Symbol &sym, const QString &result);
    bool _defintion;
    QSharedPointer<ASTNode> _expression;
    QSharedPointer<OutParametersNode> _outParms;

};
}

#endif // AssignmentNode_H
