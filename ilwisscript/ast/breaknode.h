#ifndef BREAKNODE_H
#define BREAKNODE_H

namespace Ilwis {
class BreakNode : public ASTNode
{
public:
    BreakNode();
     QString nodeType() const;
};
}

#endif // BREAKNODE_H
