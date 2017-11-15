#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "logicalexpressionparser.h"
#include "tableselector.h"

#include "selectabletable.h"

using namespace Ilwis;

SelectableTable::SelectableTable() : Table()
{
}

SelectableTable::SelectableTable(const Resource& resource) : Table(resource)
{
}

SelectableTable::~SelectableTable()
{
}


std::vector<quint32> SelectableTable::select(const QString &conditions) const
{
    return TableSelector::select(this, conditions);
}

QString SelectableTable::primaryKey() const {
    return "?";
}


