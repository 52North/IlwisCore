#include "ilwis.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "formatter.h"

using namespace Ilwis;

Formatter::Formatter() : ASTNode("Formatter")
{
}

void Formatter::setFormatNameSpace(IDNode *node)
{
    _namespace = QSharedPointer<IDNode>(node);
}

void Formatter::setFormat(char *s)
{
    QString quoted(s);
    _format = quoted.mid(1, quoted.size() - 2);
}

void Formatter::setDataType(const QString &tp)
{
    _dataType = tp;
}

QString Formatter::fnamespace() const
{
    if (_namespace.isNull())
        return sUNDEF;

    return _namespace->id();
}

QString Formatter::format() const
{
    return _format;
}

QString Formatter::dataType() const
{
    return _dataType;
}

//bool Formatter::evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx)
//{
//    return false;
//}
