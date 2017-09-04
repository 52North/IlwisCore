
#include <sstream>
#include <QString>

#include "wfsConnector_global.h"

#include "kernel.h"
#include "ilwis.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "catalogconnectorfactory.h"
#include "containerconnector.h"
#include "catalogconnector.h"
#include "wfs.h"
#include "wfsutils.h"
#include "wfsresponse.h"
#include "wfscontainerconnector.h"

using namespace Ilwis;
using namespace Wfs;

WfsContainerConnector::WfsContainerConnector()
{
}

WfsContainerConnector::WfsContainerConnector(const Resource &resource):ContainerConnector(resource)
{

}

ConnectorInterface *WfsContainerConnector::create(const Resource &res, bool)
{
    return new WfsContainerConnector(res);
}

bool WfsContainerConnector::prepare()
{
//    auto *cfactory = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", source());
//     if (!cfactory) {
//         return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Connector Factory",source().toLocalFile());
//     }
//    QList<CatalogConnector*> catalogs = cfactory->create(Resource(source().url(), itCATALOG));
//    for(CatalogConnector *catalog : catalogs) {
//        catalog->loadItems();
//    }
    QList<Resource> resources;
    Resource resource(source().url(), itCATALOG);
    resources.push_back(resource);
    mastercatalog()->addItems(resources);
    return true;
}

std::vector<QUrl> WfsContainerConnector::sources(const QStringList &filter, int options) const
{
    std::vector<QUrl> ret;
    ret.push_back(source().url());
    return ret;
}

QFileInfo WfsContainerConnector::toLocalFile(const QUrl &datasource) const
{

    // TODO: what is the intent of this method?

    // TODO: Download resource here?
    QFileInfo inf = datasource.toLocalFile();
    return inf;
}

bool WfsContainerConnector::isValid() const
{
    return source().isValid();
}

QString WfsContainerConnector::provider() const
{
    return "wfs";
}

bool WfsContainerConnector::canUse(const Resource &resource) const
{
    QString scheme = resource.url().scheme();
    if ( scheme == "ilwis")
        return false; // can't use anything marked as internal

    if ( scheme != "http" && scheme != "https")
        return false; // can't read non http based data

    IlwisTypes type = resource.ilwisType() ;
    if ( type & itCONTAINER)
        return WfsUtils::isValidWfsUrl(resource.url());

    return false;
}

