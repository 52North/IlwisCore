#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "tableoperation.h"
#include "columnvisibility.h"

using namespace Ilwis;
using namespace Desktop;

ColumnVisibility::ColumnVisibility() : TableOperation("Column Visibility", QUrl("ColumnVisibility.qml"),TR("Sets the visibility of columns"))
{

}

bool ColumnVisibility::canUse(TableModel *tblModel, const QVariantMap& parameter) const
{
    bool ok = parameter["tableonly"].toBool();
    return ok;
}


void ColumnVisibility::execute(const QVariantMap& parameters )
{
    bool ok;
    quint32 colIndex = parameters["columnindex"].toInt(&ok);

    tableModel()->update();
}

TableOperation *ColumnVisibility::create()
{
    return new ColumnVisibility();
}

