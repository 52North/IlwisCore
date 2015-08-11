#ifndef SHOWPOLYGONAREAS_H
#define SHOWPOLYGONAREAS_H


namespace Ilwis {
namespace Geodrawer {


class ShowPolygonAreas : public DrawerOperation
{
public:
    ShowPolygonAreas();
    ~ShowPolygonAreas();

    ShowPolygonAreas(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(ShowPolygonAreas);

private:
    bool _areaVisibility = true;
    int _index = iUNDEF;
    QString _code = sUNDEF;
    DrawerInterface::DrawerType _type = DrawerInterface::dtDONTCARE;
};
}
}

#endif // SHOWPOLYGONAREAS_H
