#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition() : _multiple(false), _changed(false)
{
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition &def, quint32 index) :
    Identity(def.name(), index),
    _multiple(false),
    _changed(false)
{
    datadef().domain(def.datadef().domain());
    datadef().range(def.datadef().range()->clone());

}

ColumnDefinition::ColumnDefinition(const QString &name, const DataDefinition &def, quint64 colindex) :
    Identity(name, colindex),
    _datadef(def),
    _multiple(false),
    _changed(false)
{

}

ColumnDefinition::ColumnDefinition(const QString &nm, const IDomain &dom, quint64 colindex) :
    Identity(nm, colindex),
    _multiple(false),
    _changed(false)
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

void ColumnDefinition::columnindex(quint64 idx)
{
    setId(idx);
}

quint64 ColumnDefinition::columnindex() const
{
    return id();
}

bool ColumnDefinition::isChanged() const
{
    return _changed;
}

void ColumnDefinition::changed(bool yesno)
{
    _changed = yesno;
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
