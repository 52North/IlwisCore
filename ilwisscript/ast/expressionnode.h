#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

namespace Ilwis{

typedef std::function<std::set<quint32>(const Indices& , const Indices& )> RelationFunc;

class ExpressionNode : public OperationNodeScript
{
public:
    ExpressionNode();
    QString nodeType() const;
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);

private:
    bool handleAnd(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleOr(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);
    bool handleXor(int index,const NodeValue &vright, Ilwis::SymbolTable &symbols, Ilwis::ExecutionContext *ctx);

    template<typename Func> void handleIndexes(int index, const NodeValue& vright, Func fun)
    {
        Indices index1 = _value.value(index).value<Indices>();
        Indices index2 = vright.value(index).value<Indices>();
        std::set<quint32> resultSet = fun(index1, index2);
        Indices results(resultSet.begin(), resultSet.end());
        QVariant value;
        value.setValue<Indices>(results);
        _value = {value, Identity::newAnonymousName(), NodeValue::ctLIST};

    }

};
}

#endif // EXPRESSIONNODE_H
