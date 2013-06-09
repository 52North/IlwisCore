#ifndef RANGEDEFINITIONNODE_H
#define RANGEDEFINITIONNODE_H

class RangeDefinitionNode : public ASTNode
{
public:
    RangeDefinitionNode(const QString &tp);
    void setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2);
    void setRangeType(const QString& tp);
    Ilwis::Box2D<int> box() const;
    QString variable() const;
    QString rangeType() const;

    void setVariable(const QString &v);
private:
    Ilwis::Box2D<int> _box;
    QString _variable;
    QString _rangeType;
};

#endif // RANGEDEFINITIONNODE_H
