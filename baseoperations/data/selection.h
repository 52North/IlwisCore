#ifndef SELECTION_H
#define SELECTION_H


namespace Ilwis {
namespace BaseOperations {

class SelectionRaster : public SelectionBase
{
public:
    SelectionRaster();
    SelectionRaster(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionRaster();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:
    BoundingBox _box;

    NEW_OPERATION(SelectionRaster)    ;


};

class AttributeRaster : public SelectionRaster
{
public:
    AttributeRaster();
    AttributeRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);

    static quint64 createMetadata();

    NEW_OPERATION(AttributeRaster)    ;



};

class RasterBand : public SelectionRaster
{
public:
    RasterBand();
    RasterBand(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);
    bool execute(ExecutionContext *ctx, SymbolTable &symTable);

    static quint64 createMetadata();

    NEW_OPERATION(RasterBand)    ;



};
}
}
#endif // SELECTION_H
