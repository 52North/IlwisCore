#ifndef ASTNODE_H
#define ASTNODE_H

#include <QSharedPointer>
#include <QVector>
#include <QVariant>

namespace Ilwis {
class SymbolTable;

class NodeValue : public QVariant {
public:
    enum ContentType{ctUNKNOW, ctNumerical, ctString, ctExpression, ctMethod,ctBOOLEAN, ctID};

    NodeValue();
    NodeValue(const QVariant& v, ContentType tp=NodeValue::ctUNKNOW);
    NodeValue(const QVariant& v, const QString& nid, ContentType tp=NodeValue::ctUNKNOW);

    NodeValue& operator=(QVariant a);
    NodeValue& operator=(const NodeValue& a) ;

    void setContentType(ContentType tp);
    ContentType content() const;
    QString id() const;
private:
    NodeValue::ContentType _content;
    QString _id=sUNDEF;

};
class ASTNode
{
public:
    ASTNode();
    ASTNode(const QString& t) : _type(t) {}
    virtual ~ASTNode() {}
   bool addChild(ASTNode *n);
   virtual bool evaluate(SymbolTable& symbols, int scope);
   virtual NodeValue value() const;
   bool isValid() const;
   int noOfChilderen() const;
   QSharedPointer<ASTNode> child(int i) const;
   virtual QString nodeType() const {
       return _type;
   }

protected:
    QVector<QSharedPointer<ASTNode> > _childeren;
    bool _evaluated;
    NodeValue _value;
    QString _type;

};
}

#endif // ASTNODE_H
