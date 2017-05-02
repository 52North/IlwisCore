#ifndef COLUMNVISIBILITY_H
#define COLUMNVISIBILITY_H

#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"

class TableModel;

namespace Ilwis {
namespace Desktop {
class ColumnVisibility : public TableOperation
{
    Q_OBJECT


public:
    ColumnVisibility();
    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Desktop::TableOperation *create();
};
}
}

#endif // COLUMNVISIBILITY_H
