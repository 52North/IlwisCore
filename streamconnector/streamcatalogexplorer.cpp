#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "catalogconnector.h"
#include "raster.h"
#include "catalog.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "abstractfactory.h"
#include "rawconverter.h"
#include "ilwiscontext.h"
#include "streamcatalogexplorer.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "mastercatalogcache.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

REGISTER_CATALOGEXPLORER(StreamCatalogExplorer)

StreamCatalogExplorer::StreamCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

void createCatalog(const IRasterCoverage& raster,std::vector<Resource>& items){
    Resource resCatalog = raster->resource();
    resCatalog.newId();
    resCatalog.name(raster->name());
    resCatalog.createTime(Time::now());
    resCatalog.modifiedTime(Time::now());
    resCatalog.setIlwisType(itCATALOG);
    resCatalog.setExtendedType(resCatalog.extendedType() | itRASTER);
    items.push_back(resCatalog);
    RasterStackDefinition defs = raster->stackDefinition();
    bool namedLayers = !hasType(raster->stackDefinition().domain()->ilwisType(), itNUMBER);
    for(quint32 band=0; band < raster->size().zsize(); ++band){
        Resource resBand = raster->resource();
        resBand.newId();
        resBand.createTime(Time::now());
        resBand.modifiedTime(Time::now());
        QUrl newUrl = resBand.url().toString();
        QString newName = resBand.name() + "_" + defs.index(band);
        if ( namedLayers)
            newName = defs.index(band);
        newName.remove(".ilwis");
        resBand.setUrl(newUrl.toString() + "/" + newName);
        resBand.code("band="+QString::number(band));
        items.push_back(resBand);
    }
}

std::vector<Resource> StreamCatalogExplorer::loadItems(const IOOptions &)
{
    QStringList sfilters;
    sfilters << "*.ilwis" ;
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);
    std::vector<Resource> items;
    for(auto url : files){
        QString path = url.toLocalFile();


        QFileInfo localfile = QFileInfo(url.toLocalFile());
        if ( localfile.isDir())
            continue;
        if (localfile.isFile()){
            std::vector<Resource> resources = CatalogConnector::cache()->find(url, Time(localfile.lastModified()));
            if ( resources.size() != 0){
                for(auto resource : resources)
                    items.push_back(resource);
            }else {
                QFile file(path);
                if ( file.open(QIODevice::ReadOnly)){
                    QDataStream stream(&file);

                              int qtstreamversion;

                    IlwisTypes tp, exttype;
                    QString version;

                    stream >> qtstreamversion;
                    stream.setVersion(qtstreamversion);
                    stream >> tp;
                    stream >> version;
                    stream >> exttype;
                    file.close();

                    if ( tp == itUNKNOWN)
                        continue;
                    Resource res(url, tp);
                    res.setExtendedType(exttype);
                    if ( hasType(tp,itWORKFLOW)){
                        items.push_back(res);
//                        IWorkflow wf;
//                        if(wf.prepare(res)){
//                            wf->createMetadata();
//                            Resource res2 = wf->resource();
//                            res2.code(res.code()); //code comes from other machine or possibly older instance which might have different id's
//                            items.push_back(res2);
//                        }
                    }else if (hasType(tp, itILWISOBJECT)){
                        try {
                            IIlwisObject obj(res);
                            if (obj.isValid() && obj->ilwisType() == itRASTER){
                                IRasterCoverage raster = obj.as<RasterCoverage>();
                                if ( raster->size().zsize() > 1){
                                    createCatalog(raster,items);
                                }
                            }
                        } catch (const ErrorObject& err){
                            kernel()->issues()->log(QString(TR("StreamCatalogExplorer: Error scanning object '%1'. Cause: '%2'")).arg(res.url().toString()).arg(err.message()), IssueObject::itError);
                        }
                        items.push_back(res);
                    }
                }
            }
        }
    }
//    for(auto& item : items){
//        IWorkflow wf;
//        if(item.ilwisType() == itWORKFLOW && wf.prepare(item)){
//           wf->createMetadata();
//           Resource res = wf->resource();
//           res.code(item.code());
//           item = res;
//        }
//    }
    return items;

}

IlwisObject *StreamCatalogExplorer::createType(IlwisTypes tp){
    switch(tp){
    case itFEATURE:
        return new FeatureCoverage();
    case itRASTER:
        return new RasterCoverage();
    case itTABLE:
        return new FlatTable();
    case itWORKFLOW:
        return new Workflow();
    }
    return 0;
}

bool StreamCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() == "file")
        return true;
    if (resource.url().toString() == INTERNAL_CATALOG)
        return true;
    return false;
}

QString StreamCatalogExplorer::provider() const
{
    return "ilwis"    ;
}

Ilwis::CatalogExplorer *StreamCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
    return new StreamCatalogExplorer(resource, options);
}
