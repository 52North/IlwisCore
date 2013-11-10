#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "mastercatalog.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "filecatalogconnector.h"

using namespace Ilwis;

FileCatalogConnector::FileCatalogConnector(const Resource &resource) : CatalogConnector(resource){
}

QFileInfoList FileCatalogConnector::loadFolders(const QStringList& namefilter) const
{
    QFileInfoList fileList;

    QUrl location = _location.url();
    if ( location.toString() == "file://") { // root will only contain drives (folders)
        fileList = QDir::drives();
        QFileInfoList dirs;
        foreach(QFileInfo inf , fileList) {
             QDir dir(inf.canonicalPath());
             dirs.append(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
        }
        fileList.append(dirs);;
    } else {
        QDir folder(location.toLocalFile());
        folder.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        if (!folder.exists()) {
            kernel()->issues()->log(TR(ERR_COULD_NOT_OPEN_READING_1).arg(folder.absolutePath()));
            return QFileInfoList();
        }
        fileList = folder.entryInfoList();
        folder.setFilter(QDir::Files);
        QFileInfoList files = folder.entryInfoList(namefilter);
        fileList.append(files);

    }
    return fileList;
}

Resource FileCatalogConnector::loadFolder(const QFileInfo& file, QUrl container, const QString& path, const QUrl& url) const
{
    Resource resource(url, itCATALOG);
//    //drives have the format file:///c:/ while folders have file:///c:/myfolder; note the slash
//    //drives must be recognized as container and formatted as such
//    int index = path.lastIndexOf(QRegExp("\\\\|/"));
//    if ( index != -1 && path[index - 1] == ':') { // is it a drive? (windows)
//        if ( index == path.size() - 1)
//            container = QUrl("file://");
//        else
//            container = QUrl("file:///" + path.left(3));
//    }
//    resource.setContainer(container);
//    QString name = file.isRoot() ? file.absoluteFilePath() : file.fileName();
//    resource.setName(name, false);

    return resource;
}
