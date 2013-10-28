#ifndef OUTPARAMETERSNODE_H
#define OUTPARAMETERSNODE_H

namespace Ilwis {
class Selector;

class OutParametersNode : public ASTNode
{
public:
    OutParametersNode();
    void addSelector(const QString &id, Selector *n);
    void addResult(IDNode *n);
    void addSpecifier(const QString &id, ASTNode *n);
    QSharedPointer<Selector> selector(const QString &id) const;
    QString id(int index) const;
    QSharedPointer<ASTNode> specifier(const QString &id) const;

private:
    std::vector<QSharedPointer<IDNode>> _ids;
    std::map<QString,QSharedPointer<Selector>>  _selectors;
    std::map<QString,QSharedPointer<ASTNode>> _specifiers;
};
}

#endif // OUTPARAMETERSNODE_H
