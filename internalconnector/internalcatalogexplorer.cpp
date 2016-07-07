#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QSqlRecord>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalcatalogexplorer.h"

using namespace Ilwis;
using namespace Internal;

REGISTER_CATALOGEXPLORER(InternalCatalogExplorer)

CatalogExplorer *InternalCatalogExplorer::create(const Resource& resource, const IOOptions &options) {
    if ( resource.url().scheme() == "ilwis")
        return new InternalCatalogExplorer(resource, options);
    return nullptr;

}

//InternalCatalogExplorer::InternalCatalogExplorer(const Resource& resource, bool load ) :CatalogConnector(Resource(INTERNAL_CATALOG_URL,itCATALOG), load)
InternalCatalogExplorer::InternalCatalogExplorer(const Resource& resource, const IOOptions &options) :CatalogExplorer(resource, options)
{
}

InternalCatalogExplorer::~InternalCatalogExplorer()
{

}

bool InternalCatalogExplorer::canUse(const Resource &res) const
{
    return res.url().scheme() == "ilwis";
}

QString InternalCatalogExplorer::provider() const
{
    return "internal";
}

QFileInfo InternalCatalogExplorer::toLocalFile(const QUrl& url) const {
    QFileInfo inf = url.toLocalFile();
    return inf;
}

std::vector<Resource> InternalCatalogExplorer::loadItems(const IOOptions&)
{
    std::vector<Resource> items = mastercatalog()->select(source().url(),"type<>" + QString::number(itGEODETICDATUM));
   return items;
}

bool InternalCatalogExplorer::isValid() const{
    return true;
}



