#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition(bool readOnly) : _readOnly(readOnly), _changed(false)
{
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition &def, quint32 index, bool readOnly) :
    Identity(def.name(), index),
    _readOnly(readOnly),
    _changed(false)
{
    datadef().domain(def.datadef().domain<>());
    if ( !def.datadef().range().isNull())
        datadef().range(def.datadef().range()->clone());

}

ColumnDefinition::ColumnDefinition(const QString &name, const DataDefinition &def, quint32 colindex, bool readOnly) :
    Identity(name, colindex),
    _datadef(def),
    _readOnly(readOnly),
    _changed(false)

{

}

ColumnDefinition::ColumnDefinition(const QString &name, const IDomain &dom, quint32 colindex, bool readOnly) :
    Identity(name, colindex),
    _readOnly(readOnly),
    _changed(false)
{
    datadef().domain(dom);
}

bool ColumnDefinition::isValid() const
{
   return name() != sUNDEF && datadef().domain<>().isValid();
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

bool ColumnDefinition::isReadOnly() const
{
    return _readOnly;
}

bool ColumnDefinition::equals(const ColumnDefinition &def) const
{
    if ( def.name() != name())
        return false;
    if ( datadef().domain() != def.datadef().domain())
        return false;
    return true;
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
