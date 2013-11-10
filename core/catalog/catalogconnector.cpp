#include <QString>
#include <QSharedPointer>
#include <QVector>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "identity.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "connectorfactory.h"
#include "containerconnector.h"
#include "catalog.h"
#include  "resource.h"
#include "catalogconnector.h"

using namespace Ilwis;

CatalogConnector::CatalogConnector(const Resource &resource) : _location(resource)
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ConnectorFactory",resource);

    if ( factory)
        _containerconnector.reset(dynamic_cast<ContainerConnector *>(factory->createSuitable(Resource(resource.url(), itCONTAINER))));
    else {
        kernel()->issues()->log(TR("Cann't find suitable factory for %1 ").arg(resource.name()));
    }
}

Resource &CatalogConnector::source()
{
    return _location;
}

bool CatalogConnector::isValid() const
{
    return _location.isValid() && _containerconnector;
}

std::unique_ptr<ContainerConnector> &CatalogConnector::containerConnector()
{
    return _containerconnector;
}

const std::unique_ptr<ContainerConnector> &CatalogConnector::containerConnector() const
{
    return _containerconnector;
}

ConnectorInterface *CatalogConnector::clone() const
{
//    CatalogConnector *connector = ConnectorInterface::create<CatalogConnector>(_location, "ilwis3");
//    if(!connector) {
//        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",_location.toString()));
//        return 0;
//    }

    return 0;
}
