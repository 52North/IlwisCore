#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "symboltable.h"
#include "mastercatalog.h"
#include "astnode.h"

using namespace Ilwis;

NodeValue::NodeValue() : _content(ctUNKNOW){
}

NodeValue::NodeValue(const QVariant& v, ContentType tp) : QVariant(v), _content(tp) {
    if ( tp == ctID)
        _id = v.toString();
}

NodeValue::NodeValue(const QVariant &v, const QString &nid, NodeValue::ContentType tp) :  QVariant(v), _content(tp), _id(nid)
{
}

NodeValue& NodeValue::operator=(QVariant a) {
    setValue(a);
    _content = ctUNKNOW;
    return *this;
}

NodeValue& NodeValue::operator=(const NodeValue& a) {
    operator=((QVariant)a);
    _content = a._content;
    _id = a.id();
    return *this;
}
void NodeValue::setContentType(ContentType tp) {
    _content = tp;
}

NodeValue::ContentType NodeValue::content() const {
    return _content;
}

QString NodeValue::id() const
{
    return _id;
}

QString NodeValue::toString() const
{
    if ( _content == ctMethod || _content == ctID)
        return _id;
    return QVariant::toString();
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

bool ASTNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext* ctx)
{
    foreach(QSharedPointer<ASTNode> node, _childeren) {
        if (!node->evaluate(symbols, scope, ctx)) {
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

QVariant ASTNode::resolveValue(const NodeValue &val, SymbolTable &symbols)
{
    QVariant var = val;
    if ( val.content() == NodeValue::ctID) {
        Symbol sym = symbols.getSymbol(var.toString());
        if(!sym.isValid()) {
            quint64 id = mastercatalog()->name2id(var.toString());
            if ( id == i64UNDEF)
                throw ScriptError(QString(TR(ERR_ILLEGAL_VALUE_2)).arg("parameter").arg(var.toString()));
        } else
            var = sym._var;
    }
    return var;
}
