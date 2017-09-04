#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "postgresqlconnector.h"
#include "postgresqlparameters.h"


using namespace Ilwis;
using namespace Postgresql;

PostgresqlConnector::PostgresqlConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QString str = resource.url(true).toString();
    PostgresqlParameters params (str);
    if (params.valid())
        params.toIOOptions(ioOptionsRef());
}

PostgresqlConnector::~PostgresqlConnector()
{
}

QString PostgresqlConnector::provider() const
{
    return "postgresql";
}

