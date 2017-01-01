#ifndef RELATIONNODE_H
#define RELATIONNODE_H

namespace Ilwis {
class RelationNode : public OperationNodeScript
{
public:
    RelationNode();
     QString nodeType() const;
     bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
private:
     bool handleEQ(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleNEQ(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleGREATEREQ(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleGREATER(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleLESS(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleLESSEQ(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
     bool handleImplicitCase(const QVariant &var1, const QVariant &var2, ExecutionContext *ctx, const QString &oper);
};
}

#endif // RELATIONNODE_H
