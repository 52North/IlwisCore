#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "odfitem.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "tranquilizer.h"
#include "mastercatalogcache.h"
#include "foldercatalogexplorer.h"
#include "ilwis3catalogexplorer.h"

using namespace Ilwis;
using namespace Ilwis3;

REGISTER_CATALOGEXPLORER(Ilwis3CatalogExplorer)

CatalogExplorer *Ilwis3CatalogExplorer::create(const Resource &resource,const IOOptions& options=IOOptions()) {
        return new Ilwis3CatalogExplorer(resource, options);
}

Ilwis3CatalogExplorer::Ilwis3CatalogExplorer(const Resource &resource, const IOOptions &options) : FolderCatalogExplorer(resource, true, options)
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}



std::vector<Resource> Ilwis3CatalogExplorer::loadItems(const IOOptions &)
{
    QVariantList filters = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itILWISOBJECT,"ilwis3");
    QStringList sfilters;
    for(QVariant& ext : filters)
        sfilters += "*." + ext.toString();

    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    std::unordered_map<QString, IniFile> inifiles;
    std::set<ODFItem> odfitems;
    QHash<QString, quint64> names;
    std::vector<Resource> finalList;
    UPTranquilizer trq(Tranquilizer::create(context()->runMode()));
    trq->prepare("ilwis3 connector",source().toLocalFile(),files.size());
    kernel()->issues()->silent(true);  // error messages during scan are not needed
    try{
           // we construct the list of ini files in one go so that there is a list of ini files that only need to be loaded once.
           // multiple files reusing the same ini files ( e.g. a csy) have now much faster access (already loaded)
           foreach(const QUrl& url, files) {
               QFileInfo localfile = QFileInfo(url.toLocalFile());
               if (localfile.isFile()){
                  std::vector<Resource> resources = CatalogConnector::cache()->find(url, Time(localfile.lastModified()));
                  //TODO maplist seems to be stored only once in the cache; should 2 times, so for the moment we make an exception for mpl files; resolve later
                  if ( resources.size() == 0 || localfile.suffix() == "mpl")
                       inifiles[url.toLocalFile().toLower()] = IniFile(localfile);
                   else{
                      for(auto resource : resources)
                          finalList.push_back(resource);
                   }
               }
               else{
                   Resource res(url,itCATALOG, true);
                   finalList.push_back(res);
               }
           }

           foreach(const auto& kvp, inifiles) {
               ODFItem item(kvp.second, &inifiles);
               item.modifiedTime(Time(kvp.second.modified()),true);// this is a new scan so it should take over the time of the file
               odfitems.insert(item);
               if (!trq->update(1))
                   return std::vector<Resource>();

               if ( item.isMapList()){
                   ODFItem mapList(item);
                   mapList.newId();
                   mapList.setIlwisType(itCATALOG);
                   mapList.setExtendedType(mapList.extendedType() | itRASTER);
                   odfitems.insert(mapList);
               }
               names[kvp.second.fileInfo().absoluteFilePath().toLower()] = item.id();
           }
           for(auto& resource : finalList){
               names[OSHelper::neutralizeFileName(resource.url(true).toLocalFile())] = resource.id();
           }
           std::vector<ODFItem> items;
           trq->prepare(TR("Organizing data"),"",odfitems.size()*2);
           for( const auto& item : odfitems){
               items.push_back(item);
           }


           for(ODFItem& item : items) {
               if ( item.resolveNames(names)) {
                   finalList.push_back(item);
                    trq->update(1);
               }
           }

           kernel()->issues()->silent(false);

           if ( finalList.size() > 0)
               kernel()->issues()->log(QString(TR("Added %1 objects through the ilwis3 connector")).arg( finalList.size()),IssueObject::itMessage);

           return finalList;
       }
    catch(const ErrorObject& err){
        kernel()->issues()->silent(false);
        throw err;
    }
}

bool Ilwis3CatalogExplorer::canUse(const Resource &resource) const
{
        if ( resource.ilwisType() != itCATALOG)
            return false;
        if ( resource.url().scheme() != "file") // can't use anything marked as file
            return false;
        if (resource.url().scheme() == "ilwis")
            return false;
        if ( QFileInfo(resource.url(true).toLocalFile()).isFile()) // no file can be an ilwis3 catalog.
            return false;
        return true;
}

QString Ilwis3CatalogExplorer::provider() const
{
    return "ilwis3";
}

QFileInfo Ilwis3CatalogExplorer::toLocalFile(const QUrl &datasource) const{
    return QFileInfo(datasource.toLocalFile());
}






