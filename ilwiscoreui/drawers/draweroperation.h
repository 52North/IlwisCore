#ifndef DRAWEROPERATION_H
#define DRAWEROPERATION_H

#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Geodrawer {
class ILWISCOREUISHARED_EXPORT DrawerOperation : public OperationImplementation
{
public:
    DrawerOperation();
    ~DrawerOperation();

    DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
protected:
    DrawerInterface *_rootDrawer = 0;
    quint64 _viewid = i64UNDEF;
};
}
}

#endif // DRAWEROPERATION_H
