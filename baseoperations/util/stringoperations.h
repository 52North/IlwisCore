#ifndef STRINGOPERATIONS_H
#define STRINGOPERATIONS_H

namespace Ilwis {
namespace BaseOperations {
class StringFind : public OperationImplementation
{
public:
    StringFind();

    StringFind(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text;
    QString _searchText;
    qint32 _start=0;
};

class StringSub : public OperationImplementation
{
public:
    StringSub();

    StringSub(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text;
    qint32 _end=10000000;
    qint32 _start=0;
};

class StringReplace : public OperationImplementation
{
public:
    StringReplace();

    StringReplace(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &);

    static quint64 createMetadata();

private:
    QString _text;
    QString _searchText;
    QString _replaceText;
};
}
}
#endif // STRINGOPERATIONS_H
