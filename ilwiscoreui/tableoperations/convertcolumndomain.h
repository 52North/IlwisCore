#ifndef CONVERTCOLUMNDOMAIN_H
#define CONVERTCOLUMNDOMAIN_H
#include "kernel.h"
#include "ilwisdata.h"
#include "tableoperation.h"


class TableModel;

namespace Ilwis {
namespace Desktop {
class ConvertColumnDomain : public TableOperation
{
    Q_OBJECT
public:
    ConvertColumnDomain();

    bool canUse(TableModel *tblModel, const QVariantMap &parameter) const;

    Q_INVOKABLE void execute(const QVariantMap &parameters);
    static Ilwis::Desktop::TableOperation *create();

};
}
}

#endif // CONVERTCOLUMNDOMAIN_H
