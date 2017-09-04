#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "domain.h"

#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlrasterconnector.h"
#include "postgresqlobjectfactory.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlObjectFactory::PostgresqlObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","postgresql","")
{
}

bool PostgresqlObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() != "postgresql")
        return false; // can't use anything other than pg connection

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itFEATURE)
        return true;
    else if ( type & itTABLE)
        return true;
    if ( type & itCATALOG)
        return true;
    if (type & itRASTER){
        return true;
    }
    return false;
}

IlwisObject *PostgresqlObjectFactory::create(const Resource &resource, const IOOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    IlwisObjectConnector *connector = factory->createFromResource<IlwisObjectConnector>(resource, "postgresql", options);
    if ( !connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
    }
    IlwisObject *object = createObject(connector, options);
    if ( object)
       return object;

    delete connector;
    return 0;
}
