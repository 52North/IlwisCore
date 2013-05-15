#ifndef SIN_H
#define SIN_H

namespace Ilwis {
class Sinus : public OperationImplementation
{
public:
    Sinus();
    Sinus(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);

    static quint64 createMetadata();

};
}

#endif // SIN_H
