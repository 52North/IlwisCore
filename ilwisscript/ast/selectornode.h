#ifndef Selector_H
#define Selector_H

class Selector : public ASTNode
{
public:
    Selector(const QString &tp);
    void setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2);
    void setSelectorType(const QString& tp);
    Ilwis::Box2D<int> box() const;
    QString variable() const;
    QString selectorType() const;

    void setVariable(const QString &v);
private:
    Ilwis::Box2D<int> _box;
    QString _variable;
    QString _selectorType;
};

#endif // Selector_H
