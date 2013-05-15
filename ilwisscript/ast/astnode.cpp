#include "symboltable.h"
#include "astnode.h"

NodeValue::NodeValue() : _content(ctUNKNOW){
}

NodeValue::NodeValue(const QVariant& v, ContentType tp) : QVariant(v), _content(tp) {

}

NodeValue& NodeValue::operator=(QVariant a) {
    setValue(a);
    _content = ctUNKNOW;
    return *this;
}

NodeValue& NodeValue::operator=(NodeValue a) {
    operator=((QVariant)a);
    _content = a._content;
    return *this;
}
void NodeValue::setContentType(ContentType tp) {
    _content = tp;
}

NodeValue::ContentType NodeValue::content() const {
    return _content;
}
//--------------------------------------------------------------
ASTNode::ASTNode() : _evaluated(false), _type("astnode")
{
}

bool ASTNode::addChild(ASTNode *n)
{
    _childeren.push_back(QSharedPointer<ASTNode>(n));
    return true;
}

bool ASTNode::evaluate(SymbolTable& symbols, int scope)
{
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if (!node->evaluate(symbols, scope)) {
            return false;
        }
    }
    return true;
}

NodeValue ASTNode::value() const
{
    return _value;
}

bool ASTNode::isValid() const
{
    return true;
}

int ASTNode::noOfChilderen() const
{
    return _childeren.size();
}

QSharedPointer<ASTNode> ASTNode::child(int i) const
{
    if ( i < _childeren.size())
        return _childeren[i];

    return QSharedPointer<ASTNode>();
}
