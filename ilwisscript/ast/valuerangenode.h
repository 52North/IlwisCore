#ifndef VALUERANGENODE_H
#define VALUERANGENODE_H

namespace Ilwis {


class ValueRangeNode : public ASTNode
{
public:
    ValueRangeNode();
     QString nodeType() const;
};

}
#endif // VALUERANGENODE_H
