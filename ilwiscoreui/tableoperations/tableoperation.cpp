#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "tableoperation.h"

using namespace Ilwis;
using namespace Desktop;

TableOperation::TableOperation(const QString& name, const QUrl &url, const QString& description, QObject *parent) : QObject(parent), Identity(name,i64UNDEF,sUNDEF,description), _qmlUrl(url)
{

}

void TableOperation::prepare(TableModel* tbl)
{
    _tblModel = tbl;
}

QString TableOperation::qmlUrl() const
{
    return _qmlUrl.toString();
}

ITable TableOperation::table() const
{
    return tableModel()->table();
}

TableModel *TableOperation::tableModel() const
{
    return _tblModel;
}




