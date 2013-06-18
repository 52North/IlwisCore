#ifndef EXECUTESCRIPT_H
#define EXECUTESCRIPT_H

namespace Ilwis {

class Script : public OperationImplementation
{
public:
    Script();
    Script(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable &);
    OperationImplementation::State prepare(Ilwis::ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);

    static quint64 createMetadata();
private:
    std::unique_ptr<char []> _buffer;
    quint32 _bufferSize;

};
}

#endif // EXECUTESCRIPT_H
