#include <QFileInfo>
#include <QDir>
#include "identity.h"
#include "kernel.h"
#include "mastercatalog.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "connectorfactory.h"
#include "ilwiscontext.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "foldercatalogexplorer.h"

using namespace Ilwis;

CatalogExplorer *FolderCatalogExplorer::create(const Resource &resource, bool load, const IOOptions &options) {
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url(true).toLocalFile();
        if ( localDir.path() != "." && localDir.exists()){
            return new FolderCatalogExplorer(resource, load, options);
        }
    }
    return nullptr;

}

FolderCatalogExplorer::FolderCatalogExplorer(const Resource &resource, bool , const IOOptions &options) : CatalogExplorer(resource,options){
}

std::vector<QUrl> FolderCatalogExplorer::sources(const QStringList &filters, int options ) const
{
    return FolderCatalogExplorer::loadFolders(source(), filters, options);
}

std::vector<QUrl> FolderCatalogExplorer::loadFolders(const Resource& source, const QStringList& namefilter, int options)
{
    QStringList fileList;

     QUrl location = source.url(true);
     if ( location.toString() == "file://") { // root will only contain drives (folders)
         QFileInfoList drives = QDir::drives();
         QStringList dirs;
         foreach(const auto& drive , drives) {
             QDir dir(drive.canonicalFilePath());
              dirs.append(dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
         }
         fileList.append(dirs);
     } else {
         QString p = location.toLocalFile();
         QDir folder(p);
         p = folder.absolutePath();
         folder.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
         QStringList dirlist = folder.entryList();
         if (!folder.exists()) {
             return  std::vector<QUrl>();
         }
         QString slash = location.toString().endsWith("/") ? "" : "/";
//         fileList = folder.entryList();

//         for(QString& file : fileList) {
//             file = source.url().toString() + slash + file;
//         }
         folder.setFilter(QDir::Files | QDir::Dirs);

         QStringList files = folder.entryList(namefilter);
         files.append(dirlist);

         QString sourceUrl = source.url(true).toString();

         for(QString file : files) {
             QString fullfile = sourceUrl + slash + file;
             fileList.push_back(fullfile);
         }

     }
     std::vector<QUrl> files(fileList.size());
     auto iter = files.begin();
     for(const QString& filename : fileList) {
         (*iter) = filename;
         ++iter;
     }
     return files;
}

QFileInfo FolderCatalogExplorer::toLocalFile(const QUrl& datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

QString FolderCatalogExplorer::provider() const
{
    return "ilwis";
}

std::vector<Resource> FolderCatalogExplorer::loadItems(const IOOptions&)
{
    return std::vector<Resource>();
}

bool FolderCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url(true).toLocalFile();
        if ( localDir.exists()){
            return true;
        }
    }
    return false;
}



