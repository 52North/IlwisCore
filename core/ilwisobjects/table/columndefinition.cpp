#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition()
{
}

ColumnDefinition::ColumnDefinition(const QString &nm, const IDomain &dom, quint64 colindex) : Identity(nm, colindex)
{
    datadef().domain(dom);
}

bool ColumnDefinition::isValid() const
{
   return name() != sUNDEF && datadef().domain().isValid();
}

QString ColumnDefinition::type() const
{
    return "Column";
}

const DataDefinition &ColumnDefinition::datadef() const
{
    return _datadef;
}

DataDefinition &ColumnDefinition::datadef()
{
    return _datadef;
}


