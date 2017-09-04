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
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "gdalobjectfactory.h"

using namespace Ilwis;
using namespace Gdal;

GdalObjectFactory::GdalObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","gdal","")
{
}

IlwisObject *GdalObjectFactory::create(const Resource &resource, const IOOptions &options) const
{

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    IlwisObjectConnector *connector = factory->createFromResource<IlwisObjectConnector>(resource, "gdal", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}

bool GdalObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if ( resource.url().scheme() != "file") // can't read non file based data, for the moment. In theory gdal does some services but the quality of it is soso
        return false;

    if (!gdal()->supports(resource))
        return false;

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itDOMAIN)
        return true;
    else if ( type & itCOORDSYSTEM)
        return true;
    else if ( type & itRASTER)
        return true;
    else if ( type & itGEOREF)
        return true;
    else if ( type & itFEATURE)
        return true;
    else if ( type & itTABLE)
        return true;
    else if ( type & itCATALOG)
        return true;

    return false;
}
