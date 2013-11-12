#ifndef FOLDERCONNECTOR_H
#define FOLDERCONNECTOR_H

namespace Ilwis {

class ConnectorInterface;

class FolderConnector : public ContainerConnector
{
public:
    FolderConnector();
    FolderConnector(const Resource &resource, bool);
    ~FolderConnector() {}
    std::vector<QUrl> sources(const QStringList &filters, int options=foFULLPATHS) const;
//    bool readText(const QUrl& source, QString& result,quint32 len=-1) const;
//    quint64 readBinary(char *buf, qint64 maxSize);
    QString provider() const;
    bool prepare();
    QFileInfo toLocalFile(const QUrl &datasource) const;

    static ConnectorInterface *create(const Resource &resource, bool load);
    bool canUse(const Resource &resource) const;

//    bool connectTo(const QString &filename,  ContainerConnector::OpenMode);
//    void disconnect();
private:
    std::vector<QUrl> loadFolders(const QStringList &namefilter) const;
    Resource loadFolder(const QFileInfo &file, QUrl container, const QString &path, const QUrl &url) const;

    std::vector<std::unique_ptr<CatalogConnector>> _catalogs;
    std::unique_ptr<QFile> _file;
};
}

#endif // FOLDERCONNECTOR_H
