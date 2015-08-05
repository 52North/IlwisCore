#ifndef REPRESENTATIONATTRIBUTEFILLCOLOR_H
#define REPRESENTATIONATTRIBUTEFILLCOLOR_H

namespace Ilwis {
namespace Geodrawer {


class RepresentationAttributeFillColor : public DrawerOperation
{
public:
    RepresentationAttributeFillColor();

    RepresentationAttributeFillColor(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

    NEW_OPERATION(RepresentationAttributeFillColor);

private:
    QString _layerCode = sUNDEF;
    QString _attribute = sUNDEF;
    QColor _color;
    QString _item = sUNDEF;
};
}
}

#endif // REPRESENTATIONATTRIBUTEFILLCOLOR_H
