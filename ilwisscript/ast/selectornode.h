#ifndef Selector_H
#define Selector_H

namespace Ilwis {
class Selector : public ASTNode
{
public:
    Selector(const QString &tp);
    void setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2);
    void setSelectorType(const QString& tp);
    BoundingBox box() const;
    QString variable() const;
    QString selectorType() const;
    void beginCol(const QString& col);
    QString beginCol() const;
    void endCol(const QString& col);
    QString endCol() const;
    void beginRec(const QString& col);
    quint32 beginRec() const;
    void endRec(const QString& col);
    quint32 endRec() const;
    void keyColumns(const QString& keys);
    QString keyColumns() const;

    void setVariable(const QString &v);
private:
    BoundingBox _box;
    QString _variable;
    QString _selectorType;
    QString _beginCol;
    QString _endCol;
    quint32 _beginRec;
    quint32 _endRec;
    QString _keyColumns;
};
}

#endif // Selector_H
