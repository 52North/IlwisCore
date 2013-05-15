#ifndef PLUS_H
#define PLUS_H

namespace Ilwis {
class Plus : public OperationImplementation
{
public:
    Plus();
    Plus(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool canExecute(const QString &) const;
    bool execute(ExecutionContext *ctx);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

private:
    bool executeCoverageNumber(int indexCoverage, int indexNumber, Ilwis::ExecutionContext *ctx);
    bool executeCoverageCoverage();
};
}

#endif // PLUS_H
