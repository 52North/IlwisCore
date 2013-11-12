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
    QString provider() const;
    bool prepare();
    QFileInfo toLocalFile(const QUrl &datasource) const;

    static ConnectorInterface *create(const Resource &resource, bool load);
    bool canUse(const Resource &resource) const;
    bool isValid() const;

private:
    std::vector<QUrl> loadFolders(const QStringList &namefilter) const;
    Resource loadFolder(const QFileInfo &file, QUrl container, const QString &path, const QUrl &url) const;

    std::vector<std::unique_ptr<CatalogConnector>> _catalogs;
    std::unique_ptr<QFile> _file;
};
}

#endif // FOLDERCONNECTOR_H
