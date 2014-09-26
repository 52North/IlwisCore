#ifndef SELECTABLETABLE_H
#define SELECTABLETABLE_H

#include "table.h"

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT SelectableTable: public Table
{
public:
    SelectableTable();

    SelectableTable(const Resource& resource);

    ~SelectableTable();

    std::vector<quint32> select(const QString& conditions) const;
};

}

#endif // SELECTABLETABLE_H
