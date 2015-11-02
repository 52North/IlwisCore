#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "symboltable.h"
#include "mastercatalog.h"
#include "astnode.h"

using namespace Ilwis;

NodeValue::NodeValue() : _content(ctUNKNOW){
}

NodeValue::NodeValue(const QVariant& v, ContentType tp) : _content(tp) {
    push_back(v);
    if ( tp == ctID)
        _ids.push_back(v.toString());
}

NodeValue::NodeValue(const QVariant &v, const QString &nid, NodeValue::ContentType tp) :  _content(tp)
{
    push_back(v);
    _ids.push_back(nid);
}

NodeValue& NodeValue::operator=(const QVariant& a) {
    clear();
    push_back(a);
    _content = ctUNKNOW;
    return *this;
}

NodeValue& NodeValue::operator=(const NodeValue& a) {
    clear();
    _ids.clear();
    for(auto var : a)
        push_back(var);
    _content = a._content;
    for(auto id : a._ids)
        _ids.push_back(id);
    return *this;
}
void NodeValue::setContentType(ContentType tp) {
    _content = tp;
}

void NodeValue::addValue(const QVariant &v, const QString &nid)
{
    push_back(v);
    _ids.push_back(nid);
}

NodeValue::ContentType NodeValue::content() const {
    return _content;
}

QString NodeValue::id(int index) const
{
    if ( index < _ids.size())
        return _ids[index];
    return sUNDEF;
}

QString NodeValue::toString() const
{
    if ( _content == ctMethod || _content == ctID){
        QString res;
        for(auto var : _ids){
            if ( res.size() != 0)
                res += ",";
            res += var;
        }
        return  res;
    }
    QString res;
    for(auto var : *this){
        if ( res.size() != 0)
            res += ",";
        res += var.toString();
    }
    return res;
}

bool NodeValue::canConvert(int index, int targetTypeId) const
{
    if ( index < size()) {
        return at(index).canConvert(targetTypeId);
    }
    return false;
}

qint64 NodeValue::toLongLong(int index, bool *ok) const
{
    if ( index < size()) {
        return at(index).toLongLong(ok);
    }
    return i64UNDEF;
}

qint64 NodeValue::toBool(int index) const
{
    if ( index < size()) {
        return at(index).toBool();
    }
    return false;
}

double NodeValue::toDouble(int index, bool *ok) const
{
    if ( index < size()) {
        return at(index).toDouble(ok);
    }
    return rUNDEF;
}

int NodeValue::toInt(int index, bool *ok) const
{
    if ( index < size()) {
        return at(index).toInt(ok);
    }
    return iUNDEF;
}

QString NodeValue::toString(int index) const
{
    if ( index < size()) {
        return at(index).toString();
    }
    return sUNDEF;
}

QVariant NodeValue::value(int index) const
{
    if ( index < size()) {
        return at(index);
    }
    return QVariant();
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

QVariant ASTNode::resolveValue(int index, const NodeValue &val, SymbolTable &symbols)
{
    if ( index>= val.size())
        return QVariant();

    QVariant var = val[index];
    if ( val.content() == NodeValue::ctID) {
        Symbol sym = symbols.getSymbol(var.toString());
        if(sym.isValid()) {
            QString tp = sym._var.typeName();
            if ( tp == "QVariantList"){
                QVariantList lst = sym._var.value<QVariantList>();
                var =  lst[0];
            }
            else
                var = sym._var;
        }
    }
    return var;
}
