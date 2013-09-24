#ifndef FILECATALOGCONNECTOR_H
#define FILECATALOGCONNECTOR_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FileCatalogConnector : public CatalogConnector
{
protected:
    FileCatalogConnector(const Resource &resource);

    QFileInfoList loadFolders(const QStringList &namefilter);
    Resource loadFolder(const QFileInfo &file, QUrl container, const QString &path, const QUrl &url);
};
}

#endif // FILECATALOGCONNECTOR_H
