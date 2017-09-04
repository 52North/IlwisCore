#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "catalog.h"
#include "oshelper.h"
#include "catalogserializerv1.h"

using namespace Ilwis;
using namespace Stream;

CatalogserializerV1::CatalogserializerV1(QDataStream &stream) : VersionedSerializer(stream)
{
}

bool CatalogserializerV1::loadItems(std::vector<Resource>& items ) {
    if (!VersionedSerializer::loadItems(items))
        return false;

    quint32 sz;
    _stream >> sz;
    for(int i = 0; i < sz; ++i){
        Resource resource;
        resource.load(_stream);
        items.push_back(resource);
    }
    return true;
}

bool CatalogserializerV1::store(IlwisObject *obj, const Ilwis::IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;

    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->prepare();
    QString baseurl;
    if (!options.contains("baseurl"))
        return false;
    baseurl = options["baseurl"].toString();



    std::vector<Resource> items;
    if ( options.contains("data"))
        items = catalog->items();

    if ( options.contains("operations")){
        ICatalog cat("ilwis://operations");
        std::vector<Resource> operations = cat->items();
        std::copy(operations.begin(), operations.end(), std::back_inserter(items));
    }

    _stream << (quint32)items.size();

    for(Resource resource : items){
        adaptResource(baseurl, resource);
        resource.store(_stream);

    }
    _stream << catalog->parentCatalog();

    return true;


}

void CatalogserializerV1::adaptProperyResource(const QString& baseUrl, Resource& resource, const QString& propertyName) const
{
    Resource properyResource = resource.property2Resource(propertyName, itCOORDSYSTEM);
    if ( properyResource.isValid()){
        QString url = adaptedUrl(baseUrl, properyResource,true);
        resource.addProperty(propertyName, url);
    }
}

void CatalogserializerV1::adaptResource(const QString& baseUrl, Resource& resource) const{

    QString tempName = resource.name();
    resource.setUrl(adaptedUrl(baseUrl, resource, false));
    resource.setUrl(adaptedUrl(baseUrl, resource, true), true);
//    QString container = QString(baseUrl).arg(resource.ilwisType() == itOPERATIONMETADATA ? "operation" : "data").arg("catalog");
//    resource.addContainer(QUrl(container));
    resource.name(tempName, false);
    adaptProperyResource(baseUrl, resource,"coordinatesystem");
    adaptProperyResource(baseUrl, resource,"georeference");
    adaptProperyResource(baseUrl, resource,"domain");
}

QString CatalogserializerV1::adaptedUrl(const QString& baseUrl, const Resource& resource, bool asRaw) const{
    QString url;
    if ( asRaw){
        QString sourceurl = resource.url(true).toString();
        QString tail = sourceurl.mid(sourceurl.lastIndexOf("/") + 1);
        url = QString(baseUrl).arg(tail).arg(IlwisObject::type2Name(resource.ilwisType()).toLower());
        if ( resource.ilwisType() == itOPERATIONMETADATA){
            url = url.replace("/dataaccess?","/operation?");
            url = url.replace("?datasource=","?operationcode=");
        }
    }else {
        QUrl burl(baseUrl);
        QString host = burl.host();
        int port = burl.port();
        QString path = resource.ilwisType() == itOPERATIONMETADATA ? "" : burl.path();
        url = QString("http://%1:%2%3/%4").arg(host).arg(port).arg(path).arg(resource.name());
    }
    return url;
}

bool CatalogserializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if ( hasType(obj->resource().extendedType(), itRASTER)){
        _stream.device()->seek(0); // reset the position for next reads
        return true;
    }
    std::vector<Resource> items;
    if(!loadItems(items))
        return false;

    mastercatalog()->addItems(items);

    QUrl url;
    _stream >> url;
    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->setParentCatalog(QUrl(url));

    return true;
}

VersionedSerializer *CatalogserializerV1::create(QDataStream &stream)
{
        return new CatalogserializerV1(stream);
}
