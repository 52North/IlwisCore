#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "filecatalogconnector.h"
#include "catalog.h"
#include "dataformat.h"
#include "gdalmodule.h"
#include "gdalproxy.h"
#include "gdalcatalogconnector.h"
#include "gdalitem.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *GdalCatalogConnector::create(const Resource &resource,bool load) {
    return new GdalCatalogConnector(resource, load);

}

GdalCatalogConnector::GdalCatalogConnector(const Ilwis::Resource &resource, bool load) : CatalogConnector(resource,load), _type(resource.ilwisType())
{
}

bool GdalCatalogConnector::prepare()
{
    return CatalogConnector::prepare();
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

bool GdalCatalogConnector::loadItems()
{
    QStringList filters = gdal()->getRasterExtensions();
    std::multimap<QString, DataFormat>  formats = DataFormat::getSelectedBy(DataFormat::fpEXTENSION, "connector='gdal'");
    for(const auto& element : formats){
        for(const auto& ext : element.second.property(DataFormat::fpEXTENSION).toString().split(",",QString::SkipEmptyParts)){
                filters += "*." + ext;
        }
    }
    filters.removeOne("*.hdr");
    filters.removeDuplicates();

    std::vector<QUrl> files = sources(filters ,CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    QSet<Resource> gdalitems;
    QList<Resource> folders;
    if (!gdal()->isValid()) {
        return ERROR1(ERR_NO_INITIALIZED_1,"gdal library");
    }
    for(const QUrl& url : files) {
        if ( mastercatalog()->url2id(url, itRASTER) == i64UNDEF && mastercatalog()->url2id(url, itFEATURE) == i64UNDEF) {
            QFileInfo file = containerConnector()->toLocalFile(url);
            if ( !file.isDir() ) {
                IlwisTypes extendedTypes = extendedType(formats, file.suffix());
                GDALItems items(url, containerConnector(), extendedTypes);
                gdalitems += items;
            } else {
                Resource resource(url, itCATALOG);
                QString filename = file.isRoot() ? file.absoluteFilePath() : file.fileName();
                resource.setName(filename);

                folders.push_back(resource);
            }
        }

    }
    mastercatalog()->addItems(gdalitems.toList());
    mastercatalog()->addItems(folders);

    return true;
}

IlwisTypes GdalCatalogConnector::extendedType(const std::multimap<QString, DataFormat>& formats, const QString& ext) const{
    IlwisTypes types= itUNKNOWN;
    auto collection = formats.equal_range(ext);
    for(auto iter = collection.first; iter != collection.second; ++iter){
        types |= (*iter).second.property(DataFormat::fpEXTENDEDTYPE).toULongLong();
    }
    return types;
}

bool GdalCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    return true;
}

QString GdalCatalogConnector::provider() const
{
    return "gdal";
}

std::vector<QUrl> GdalCatalogConnector::sources(const QStringList &filter, int options) const
{
    //TODO for non-file based systems
    return FileCatalogConnector::loadFolders(source(), filter, options);
}

QFileInfo GdalCatalogConnector::toLocalFile(const QUrl &datasource) const{
    //TODO for non-file based systems
    return QFileInfo(datasource.toLocalFile());
}


