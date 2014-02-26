#ifndef IFOPERATION_H
#define IFOPERATION_H

namespace Ilwis {
namespace BaseOperations {
class IfOperation : public OperationImplementation
{
protected:
    IfOperation();
    
    IfOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    DataDefinition findOutputDataDef(const OperationExpression &expr);
    DataDefinition findParameterDataDef(const OperationExpression &expr, int index);
    
    ICoverage _coverages[2];
    double _number[2];

};
}
}

#endif // IFOPERATION_H
