#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

namespace Ilwis {
class OperationNodeScript : public ASTNode
{
public:
    enum Operators{oNONE, oADD, oSUBSTRACT, oMOD, oTIMES, oDIVIDED, oAND, oOR, oXOR, oLESS, oLESSEQ, oNEQ, oEQ, oGREATER, oGREATEREQ};
    struct RightTerm{
        QSharedPointer<ASTNode> _rightTerm;
        Operators _operator;
    };


    OperationNodeScript();
    void setLeftTerm(ASTNode *node);
    void addRightTerm(OperationNodeScript::Operators op, ASTNode *node);
    bool evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx);
    bool isValid() const;


protected:
    bool handleBinaryCases(int index, const NodeValue &vright, const QString& operation, const QString &relation,
                                   Ilwis::SymbolTable &symbols, ExecutionContext *ctx);
    bool handleTableCases(int index, const NodeValue &vright, const QString &operation, const QString &relation,
                          SymbolTable &symbols, ExecutionContext *ctx);
    IlwisTypes typesUsed(int index, const NodeValue &vright, SymbolTable &symbols) const;

    QSharedPointer<ASTNode> _leftTerm;
    QVector< RightTerm > _rightTerm;


private:
    QString additionalInfo(ExecutionContext *ctx, const QString &key) const;
};}

#endif // OPERATIONNODE_H
