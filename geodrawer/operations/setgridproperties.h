#ifndef SETGRIDPROPERTIES_H
#define SETGRIDPROPERTIES_H

namespace Ilwis {
namespace Geodrawer {


class SetGridProperties : public DrawerOperation
{
public:
    SetGridProperties();

    SetGridProperties(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetGridProperties);

private:
    double _distance = -1;
    double _opacity = -1;
    QColor _gridColor;
    bool usePrimary = true;

};

}
}

#endif // SETGRIDPROPERTIES_H
