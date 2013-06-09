#include "kernel.h"
#include "geometries.h"
#include "astnode.h"
#include "selectornode.h"

Selector::Selector(const QString& tp) : ASTNode(tp)
{
    _box = Ilwis::Box2D<int>(Ilwis::Point2D<int>(0,0), Ilwis::Point2D<int>(0,0));
}

void Selector::setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2){
    _box = Ilwis::Box2D<int>(QString("(%1 %2, %3 %4)").arg(x1).arg(y1).arg(x2).arg(y2));
}

void Selector::setSelectorType(const QString& tp){
    _selectorType = tp;
}

Ilwis::Box2D<int> Selector::box() const{
    return _box;

}

void Selector::setVariable(const QString& v) {
    _variable = v;
}

QString Selector::variable() const{
    return _variable;
}
QString Selector::selectorType() const{
    return _selectorType;
}
