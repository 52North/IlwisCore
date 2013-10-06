#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition() : _multiple(false)
{
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition &def, quint32 index) : Identity(def.name(), index), _multiple(false)
{
    datadef().domain(def.datadef().domain());
    datadef().range(def.datadef().range()->clone());

}

ColumnDefinition::ColumnDefinition(const QString &nm, const IDomain &dom, quint64 colindex) : Identity(nm, colindex), _multiple(false)
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

bool ColumnDefinition::isMultiple() const
{
    return _multiple;
}

void ColumnDefinition::multiple(bool yesno)
{
    _multiple = yesno;
}


//---------------------------------------------------------

bool Ilwis::operator==(const ColumnDefinition &def1, const ColumnDefinition &def2)
{
    return def1.datadef() == def2.datadef();
}

bool Ilwis::operator!=(const ColumnDefinition &def1, const ColumnDefinition &def2)
{
    return !operator==(def1, def2);
}
