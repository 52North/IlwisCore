#ifndef SORTCOLUMN_H
#define SORTCOLUMN_H

#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"

class TableModel;

namespace Ilwis {
namespace Desktop {
class SortColumn : public TableOperation
{
    Q_OBJECT


public:
    SortColumn();
    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Desktop::TableOperation *create();
};
}
}

#endif // SORTCOLUMN_H
