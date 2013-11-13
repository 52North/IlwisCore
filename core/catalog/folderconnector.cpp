#include <QDir>
#include "kernel.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "catalogconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnector.h"
#include "catalogconnectorfactory.h"
#include "folderconnector.h"

using namespace Ilwis;

ConnectorInterface *FolderConnector::create(const Resource& resource, bool load) {
    return new FolderConnector(resource, load);

}

FolderConnector::FolderConnector()
{
}

FolderConnector::FolderConnector(const Resource &resource, bool) : ContainerConnector(resource){
}

std::vector<QUrl> FolderConnector::sources(const QStringList &filters, int ) const
{
    return loadFolders(filters);
}

QString FolderConnector::provider() const
{
    return "ilwis";
}

bool FolderConnector::prepare()
{
    auto *cfactory = kernel()->factory<CatalogConnectorFactory>("catalogconnectorfactory", source());
     if (!cfactory) {
         return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Connector Factory",source().toLocalFile());
     }
     QList<CatalogConnector*> catalogs = cfactory->create(Resource(source().url(), itCATALOG));
     if ( catalogs.size() == 0) {
         return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Catalog connector", source().toLocalFile());
     }
     _catalogs.resize(catalogs.size());
     int  i =0;
     for(CatalogConnector *catalog : catalogs) {
         catalog->loadItems();
         _catalogs[i++].reset(catalog);
     }


     return true;
}

QFileInfo FolderConnector::toLocalFile(const QUrl& datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

std::vector<QUrl> FolderConnector::loadFolders(const QStringList& namefilter) const
{
    QStringList fileList;

    QUrl location = source().url();
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
            file = source().url().toString() + "/" + file;
        }
        folder.setFilter(QDir::Files);
        QStringList files = folder.entryList(namefilter);
        for(QString file : files) {
            QString fullfile = source().url().toString() + "/" +  file;
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

Resource FolderConnector::loadFolder(const QFileInfo& file, QUrl container, const QString& path, const QUrl& url) const
{
    Resource resource(url, itCATALOG);
    //drives have the format file:///c:/ while folders have file:///c:/myfolder; note the slash
    //drives must be recognized as container and formatted as such
    int index = path.lastIndexOf(QRegExp("\\\\|/"));
    if ( index != -1 && path[index - 1] == ':') { // is it a drive? (windows)
        if ( index == path.size() - 1)
            container = QUrl("file://");
        else
            container = QUrl("file:///" + path.left(3));
    }
    resource.addContainer(container);
    QString name = file.isRoot() ? file.absoluteFilePath() : file.fileName();
    resource.setName(name, false);

    return resource;
}

bool FolderConnector::canUse(const Resource& resource) const  {
    if (resource.url().scheme() != "file")
        return false;
    QFileInfo inf(resource.url().toLocalFile());
    if ( inf.isDir())
        return true;
    //if ( inf.exists()) {
    QDir dir(inf.absolutePath());
    if ( dir.exists())
        return true;

    //}
    return false;
}

bool FolderConnector::isValid() const
{
    QFileInfo inf(source().url().toLocalFile());
    return inf.exists();
}

