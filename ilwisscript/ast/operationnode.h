#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

namespace Ilwis {
class OperationNode : public ASTNode
{
public:
    enum Operators{oNONE, oADD, oSUBSTRACT, oMOD, oTIMES, oDIVIDED, oAND, oOR, oXOR, oLESS, oLESSEQ, oNEQ, oEQ, oGREATER, oGREATEREQ};
    struct RightTerm{
        QSharedPointer<ASTNode> _rightTerm;
        Operators _operator;
    };


    OperationNode();
    void setLeftTerm(ASTNode *node);
    void addRightTerm(OperationNode::Operators op, ASTNode *node);
    bool evaluate(SymbolTable& symbols, int scope);
    bool isValid() const;


protected:
    bool handleBinaryCoverageCases(const NodeValue &vright, const QString& operation, const QString &relation, Ilwis::SymbolTable &symbols);

    QSharedPointer<ASTNode> _leftTerm;
    QVector< RightTerm > _rightTerm;


};}

#endif // OPERATIONNODE_H
