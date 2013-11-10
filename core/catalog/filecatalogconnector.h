#ifndef FILECATALOGCONNECTOR_H
#define FILECATALOGCONNECTOR_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FileCatalogConnector : public CatalogConnector
{
protected:
    FileCatalogConnector(const Resource &resource);

    QFileInfoList loadFolders(const QStringList &namefilter) const;
    Resource loadFolder(const QFileInfo &file, QUrl container, const QString &path, const QUrl &url) const;
};
}

#endif // FILECATALOGCONNECTOR_H
