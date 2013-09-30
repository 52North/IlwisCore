#ifndef AssignmentNode_H
#define AssignmentNode_H

namespace Ilwis {
class AssignmentNode : public ASTNode
{
public:
    AssignmentNode();
    bool isDefinition() const;
    void setDefintion(bool yesno);
    void setResult(IDNode *node);
    //void setDomPart(ASTNode *node);
    void setExpression(ExpressionNode *node);
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void setFormatPart(ASTNode *node);

private:
    template<typename T1> bool copyObject(const Symbol& sym, const QString& name,SymbolTable &symbols) {
        IlwisData<T1> coverage =  sym._var.value<IlwisData<T1>>();
        if (!coverage.isValid())
            return false;
        bool wasAnonymous = coverage->isAnonymous();
        T1 *obj = coverage->copy();
        if(!obj)
            return false;
        obj->setName(name);
        IlwisData<T1> gcovCopy;
        gcovCopy.set(obj);
        QVariant var;
        var.setValue<IlwisData<T1>>(gcovCopy);
        symbols.addSymbol(name, 1000, obj->ilwisType(), var);
        if ( wasAnonymous)
            mastercatalog()->addItems({gcovCopy->source(IlwisObject::cmINPUT | IlwisObject::cmEXTENDED)});

        return true;
    }
    IIlwisObject getObject(const Symbol &sym) const;
    void getFormat(ASTNode *node, QString &format, QString &fnamespace) const;
    void store2Format(ASTNode *node, const Symbol &sym, const QString &result);
    bool _defintion;
    QSharedPointer<IDNode> _result;
    QSharedPointer<ASTNode> _typemodifier;
    QSharedPointer<ExpressionNode> _expression;
};
}

#endif // AssignmentNode_H
