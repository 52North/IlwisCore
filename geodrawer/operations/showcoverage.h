#ifndef SHOWCOVERAGE_H
#define SHOWCOVERAGE_H

namespace Ilwis {
namespace Geodrawer {

class ShowCoverage : public DrawerOperation
{
public:
    ShowCoverage();

    ShowCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

     NEW_OPERATION(ShowCoverage);
private:
    quint64 _id;
    QString _url;
    QString _side;
    QString _type;
};
}
}

#endif // SHOWCOVERAGE_H
