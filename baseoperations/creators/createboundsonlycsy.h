#ifndef CREATEBOUNDSONLYCSY_H
#define CREATEBOUNDSONLYCSY_H


namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace BaseOperations {
class CreateBoundsOnlyCsy :  public OperationImplementation
{
public:
    CreateBoundsOnlyCsy();

    CreateBoundsOnlyCsy(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateBoundsOnlyCsy);

private:
    double _minx = rUNDEF, _miny=rUNDEF, _maxx=rUNDEF, _maxy=rUNDEF;
    QString _description;
    QString _name;

};
}
}

#endif // CREATEBOUNDSONLYCSY_H
