#include <QSqlQuery>
#include <QObject>
#include <QStringList>
#include <QSqlError>
#include <QHash>
#include <fstream>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "geometries.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "table.h"

#include "wfsparsingcontext.h"
#include "wfsfeatureconnector.h"
#include "wfsobjectfactory.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsObjectFactory::WfsObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","wfs","")
{
}

IlwisObject *WfsObjectFactory::create(const Resource &resource, const IOOptions &options) const
{
     const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
     WfsFeatureConnector *connector = factory->createFromResource<WfsFeatureConnector>(resource, "wfs");

    if(!connector) {
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
        return 0;
    }
    IlwisObject *object = createObject(connector,options);
    if ( object)
        return object;

    delete connector;
    return 0;
}

bool WfsObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis")
        return false; // can't use anything marked as internal

    if (!WfsUtils::isValidWfsUrl(resource.url(true)))
        return false;

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itFEATURE)
        return true;
    else if ( type & itTABLE)
        return true;

    return false;
}

