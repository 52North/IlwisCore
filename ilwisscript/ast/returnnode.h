#ifndef RETURNNODE_H
#define RETURNNODE_H

class ReturnNode : public ASTNode
{
public:
    ReturnNode();

    QString nodeType() const;
};

#endif // RETURNNODE_H
