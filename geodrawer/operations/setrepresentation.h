#ifndef SETREPRESENTATION_H
#define SETREPRESENTATION_H


namespace Ilwis {

class Representation;
typedef IlwisData<Representation> IRepresentation;

namespace Geodrawer {


class SetRepresentation : public DrawerOperation
{
public:
    SetRepresentation();

    SetRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(SetRepresentation);

private:
    QString _layerCode;
    Ilwis::IRepresentation _representation;
    QString _attributeName;

};
}
}

#endif // SETREPRESENTATION_H
