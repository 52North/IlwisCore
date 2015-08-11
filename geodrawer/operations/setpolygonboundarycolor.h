#ifndef SETPOLYGONBOUNDARYCOLOR_H
#define SETPOLYGONBOUNDARYCOLOR_H


namespace Ilwis {
namespace Geodrawer {


class SetPolygonBoundaryColor : public DrawerOperation
{
public:
    SetPolygonBoundaryColor();
    ~SetPolygonBoundaryColor();

    SetPolygonBoundaryColor(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetPolygonBoundaryColor);

private:
    QColor _boundaryColor = true;
    int _index = iUNDEF;
    QString _code = sUNDEF;
    DrawerInterface::DrawerType _type = DrawerInterface::dtDONTCARE;
};
}
}

#endif // SETPOLYGONBOUNDARYCOLOR_H
