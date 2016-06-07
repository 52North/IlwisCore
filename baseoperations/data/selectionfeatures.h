#ifndef SELECTIONFEATURES_H
#define SELECTIONFEATURES_H

#include "geos/geom/geometry.h"
#include "geometries.h"
#include <memory>

namespace Ilwis {
namespace BaseOperations {

class SelectionFeatures : public SelectionBase
{
public:
    SelectionFeatures();
    SelectionFeatures(quint64 metaid, const Ilwis::OperationExpression &expr);
    ~SelectionFeatures();

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable&);

    static quint64 createMetadata();
private:



    bool _asIndex = false;


    NEW_OPERATION(SelectionFeatures);

 };
}
}

#endif // SELECTIONFEATURES_H
