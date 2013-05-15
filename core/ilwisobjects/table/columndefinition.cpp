#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "valuedefiner.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition()
{
}

ColumnDefinition::ColumnDefinition(const QString &nm, const IDomain &dom, quint64 colindex) : Identity(nm, colindex)
{
    setDomain(dom);
}

bool ColumnDefinition::isValid() const
{
   return name() != sUNDEF && domain().isValid();
}

QString ColumnDefinition::type() const
{
    return "Column";
}


