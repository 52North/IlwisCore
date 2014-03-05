#include <QFileInfo>
#include <QDir>
#include "identity.h"
#include "kernel.h"
#include "mastercatalog.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "ilwiscontext.h"
#include "filecatalogconnector.h"

using namespace Ilwis;

ConnectorInterface *FileCatalogConnector::create(const Resource &resource, bool load) {
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url().toLocalFile();
        if ( localDir.path() != "." && localDir.exists()){
            return new FileCatalogConnector(resource, load);
        }
    }
    return nullptr;

}

FileCatalogConnector::FileCatalogConnector(const Resource &resource, bool load) : CatalogConnector(resource, load){
}

std::vector<QUrl> FileCatalogConnector::sources(const QStringList &filters, int options ) const
{
    return FileCatalogConnector::loadFolders(source(), filters, options);
}

std::vector<QUrl> FileCatalogConnector::loadFolders(const Resource& source, const QStringList& namefilter, int options)
{
    QStringList fileList;

     QUrl location = source.url();
     if ( location.toString() == "file://") { // root will only contain drives (folders)
         QFileInfoList drives = QDir::drives();
         QStringList dirs;
         foreach(const auto& drive , drives) {
             QDir dir(drive.canonicalFilePath());
              dirs.append(dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
         }
         fileList.append(dirs);
     } else {
         QDir folder(location.toLocalFile());
         folder.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
         if (!folder.exists()) {
             kernel()->issues()->log(TR(ERR_COULD_NOT_OPEN_READING_1).arg(folder.absolutePath()));
             return  std::vector<QUrl>();
         }
         fileList = folder.entryList();
         for(QString& file : fileList) {
             file = source.url().toString() + "/" + file;
         }
         folder.setFilter(QDir::Files);
         QStringList files = folder.entryList(namefilter);
         for(QString file : files) {
             QString fullfile = source.url().toString() + "/" +  file;
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

QFileInfo FileCatalogConnector::toLocalFile(const QUrl& datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

bool FileCatalogConnector::loadItems()
{
    //TODO for reading catalogs

    return false;
}

QString FileCatalogConnector::provider() const
{
    return "ilwis";
}

bool FileCatalogConnector::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url().toLocalFile();
        if ( localDir.exists()){
            return true;
        }
    }
    return false;
}

bool FileCatalogConnector::prepare()
{
    return CatalogConnector::prepare();
}

