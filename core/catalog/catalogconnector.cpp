#include <QString>
#include <QSharedPointer>
#include <QVector>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalog.h"

using namespace Ilwis;

CatalogConnector::CatalogConnector(const Resource &resource, bool load ) : IlwisObjectConnector(resource, load)
{
}

bool CatalogConnector::isValid() const
{
    return source().isValid();
}

Ilwis::IlwisObject *CatalogConnector::create() const
{
    return new Catalog(source());
}

bool CatalogConnector::prepare()
{
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Connector Factory",source().toLocalFile());
    }

    QList<CatalogConnector*> catalogs = factory->connectorsForResource<CatalogConnector>(source());
    if ( catalogs.size() == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Catalog connector", source().toLocalFile());
    }
    _dataProviders.resize(catalogs.size());
    int  i =0;
    for(CatalogConnector *catalog : catalogs) {
        if (catalog->loadItems())
           _dataProviders[i++].reset(catalog);
        else
            delete catalog;
    }


    return true;
}


