#ifndef ASTNODE_H
#define ASTNODE_H

#include <QSharedPointer>
#include <QVector>
#include <QVariant>

namespace Ilwis {
class SymbolTable;
struct ExecutionContext ;

class NodeValue : public QVariantList {
public:
    enum ContentType{ctUNKNOW, ctNumerical, ctString, ctExpression, ctMethod,ctBOOLEAN, ctID, ctLIST, ctCoordLists};

    NodeValue();
    NodeValue(const QVariant& v, ContentType tp=NodeValue::ctUNKNOW);
    NodeValue(const QVariant& v, const QString& nid, ContentType tp=NodeValue::ctUNKNOW);

    NodeValue& operator=(const QVariant& a);
    NodeValue& operator=(const NodeValue& a) ;

    void setContentType(ContentType tp);
    void addValue(const QVariant& v, const QString& nid);
    ContentType content() const;
    QString id(int index=0) const;
    QString toString() const;
    bool canConvert(int index, int targetTypeId) const;
    qint64 toLongLong(int index, bool *ok=0) const;
    qint64 toBool(int index) const;
    double toDouble(int index, bool *ok=0) const;
    int toInt(int index, bool *ok=0) const;
    QString toString(int index) const;
    QVariant value(int index=0) const;

private:
    NodeValue::ContentType _content;
    QList<QString> _ids;

};
class ASTNode
{
public:
    ASTNode();
    ASTNode(const QString& t) : _type(t) {}
    virtual ~ASTNode() {}
   bool addChild(ASTNode *n);
   virtual bool evaluate(SymbolTable& symbols, int scope, ExecutionContext* ctx);
   virtual NodeValue value() const;
   bool isValid() const;
   int noOfChilderen() const;
   QSharedPointer<ASTNode> child(int i) const;
   virtual QString nodeType() const {
       return _type;
   }

protected:
    QVariant resolveValue(int index, const NodeValue &value, SymbolTable& symbols);
    QVector<QSharedPointer<ASTNode> > _childeren;
    bool _evaluated;
    NodeValue _value;
    QString _type;

};
}

Q_DECLARE_METATYPE(Ilwis::NodeValue);


#endif // ASTNODE_H
