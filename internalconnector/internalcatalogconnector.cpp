#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QSqlRecord>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "internalcatalogconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalCatalogConnector::create(const Resource& resource, bool load ) {
    if ( resource.url().scheme() == "ilwis")
        return new InternalCatalogConnector(resource, load);
    return nullptr;

}

//InternalCatalogConnector::InternalCatalogConnector(const Resource& resource, bool load ) :CatalogConnector(Resource(QUrl("ilwis://internalcatalog"),itCATALOG), load)
InternalCatalogConnector::InternalCatalogConnector(const Resource& resource, bool load ) :CatalogConnector(resource, load)
{
}

InternalCatalogConnector::~InternalCatalogConnector()
{

}

std::vector<QUrl> InternalCatalogConnector::sources(const QStringList &, int ) const
{
    //TODO: full list??
    return std::vector<QUrl>();
}



bool InternalCatalogConnector::canUse(const Resource &res) const
{
    return res.url().scheme() == "ilwis";
}

QString InternalCatalogConnector::provider() const
{
    return "internal";
}

QFileInfo InternalCatalogConnector::toLocalFile(const QUrl& url) const {
    QFileInfo inf = url.toLocalFile();
    return inf;
}

bool InternalCatalogConnector::loadItems()
{
    return true;
}

bool InternalCatalogConnector::isValid() const{
    return true;
}



