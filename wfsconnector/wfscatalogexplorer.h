#ifndef WFSCATALOGCONNECTOR_H
#define WFSCATALOGCONNECTOR_H

#include "wfsconnector_global.h"

namespace Ilwis {

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCatalogExplorer : public CatalogExplorer
{
public:

    WfsCatalogExplorer(const Resource &resource, const IOOptions &options=IOOptions());
    ~WfsCatalogExplorer();

    static Ilwis::CatalogExplorer *create(const Resource &resource,const IOOptions& options=IOOptions());

    virtual std::vector<Resource> loadItems(const IOOptions& options=IOOptions());

    bool canUse(const Resource& resource) const;

    QString provider() const;

    /*
     * does not have any purpose in the WFS catalog connector
     */
    QFileInfo toLocalFile(const QUrl& ) const {
        QFileInfo fileInfo;
        return fileInfo;
    }

private:

    NEW_CATALOGEXPLORER(WfsCatalogExplorer);

};
}
}

#endif // WFSCATALOGCONNECTOR_H
