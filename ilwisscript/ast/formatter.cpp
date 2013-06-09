#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatnode.h"

FormatNode::FormatNode() : ASTNode("formatnode")
{
}

void FormatNode::setFormatNameSpace(IDNode *node)
{
    _namespace = QSharedPointer<IDNode>(node);
}

void FormatNode::setFormat(char *s)
{
    QString quoted(s);
    _format = quoted.mid(1, quoted.size() - 2);
}

void FormatNode::setDataType(const QString &tp)
{
    _dataType = tp;
}

QString FormatNode::fnamespace() const
{
    if (_namespace.isNull())
        return sUNDEF;

    return _namespace->id();
}

QString FormatNode::format() const
{
    return _format;
}

QString FormatNode::dataType() const
{
    return _dataType;
}

//bool FormatNode::evaluate(SymbolTable &symbols, int scope)
//{
//    return false;
//}
