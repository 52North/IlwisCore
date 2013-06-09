#include "kernel.h"
#include "geometries.h"
#include "astnode.h"
#include "rangedefinitionnode.h"

RangeDefinitionNode::RangeDefinitionNode(const QString& tp) : ASTNode(tp)
{
    _box = Ilwis::Box2D<int>(Ilwis::Point2D<int>(0,0), Ilwis::Point2D<int>(0,0));
}

void RangeDefinitionNode::setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2){
    _box = Ilwis::Box2D<int>(QString("(%1 %2, %3 %4)").arg(x1).arg(y1).arg(x2).arg(y2));
}

void RangeDefinitionNode::setRangeType(const QString& tp){
    _rangeType = tp;
}

Ilwis::Box2D<int> RangeDefinitionNode::box() const{
    return _box;

}

void RangeDefinitionNode::setVariable(const QString& v) {
    _variable = v;
}

QString RangeDefinitionNode::variable() const{
    return _variable;
}
QString RangeDefinitionNode::rangeType() const{
    return _rangeType;
}
