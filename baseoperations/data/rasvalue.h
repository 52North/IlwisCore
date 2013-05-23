#ifndef RASVALUE_H
#define RASVALUE_H

class RasValue : public OperationImplementation
{
public:
    RasValue();
    RasValue(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();
private:
    IGridCoverage _inputGC;
};

#endif // RASVALUE_H
