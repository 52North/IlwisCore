#ifndef CREATECORNERSGEOREFERENCE_H
#define CREATECORNERSGEOREFERENCE_H


namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace BaseOperations {
class CreateCornersGeoreference :  public OperationImplementation
{
public:
    CreateCornersGeoreference();

    CreateCornersGeoreference(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx,const SymbolTable&);

    static quint64 createMetadata();

    NEW_OPERATION(CreateCornersGeoreference);

private:
    double _minx = rUNDEF, _miny=rUNDEF, _maxx=rUNDEF, _maxy=rUNDEF;
    double _pixelsize;
    QString _description;
    bool _centerOfPixels = false;
    ICoordinateSystem _csy;
    QString _name;

};
}
}
#endif // CREATECORNERSGEOREFERENCE_H
