#ifndef MASTERCATALOG_H
#define MASTERCATALOG_H

#include <QMultiMap>
#include <set>
#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class CatalogConnector;


typedef QExplicitlySharedDataPointer<IlwisObject> ESPObject;

class KERNELSHARED_EXPORT MasterCatalog
{
    friend KERNELSHARED_EXPORT MasterCatalog* mastercatalog();

public:
    MasterCatalog();
    ~MasterCatalog();

    bool prepare();

    bool addContainer(const QUrl &location);
    bool addItems(const QList<Resource> &items);
    bool removeItems(const QList<Resource> &items);
    quint64 resource2id(const QUrl& url, IlwisTypes tp) const;
    Resource name2Resource(const QString& name, IlwisTypes tp = itUNKNOWN) const;
    Resource id2Resource(quint64 iid) const;
    //QUrl id2url(quint64 id) const;
    IlwisTypes id2type(quint64 id) const;
    quint64 createId() const;
    void registerObject(ESPObject &data);
    bool isRegistered(quint64 id) const;
    bool unregister(quint64);
    std::list<Resource> select(const QUrl& res, const QString& selection) const;

    QUrl name2url(const QString &name, IlwisTypes tp=itUNKNOWN) const;
    ESPObject get(const QUrl &resource, IlwisTypes type) const;
    ESPObject get(quint64 id) const;
    bool contains(const QUrl &url, IlwisTypes type) const;

private:
    static MasterCatalog *_masterCatalog;
    quint64 _baseid;
    QHash<quint64, ESPObject> _lookup;
    QMultiMap<QUrl, CatalogConnector*  > _catalogs;
    std::set<uint> _knownHashes;


};

//typedef QHash<IlwisResource, QList<CatalogCreate>  > CatalogCollection;

KERNELSHARED_EXPORT MasterCatalog* mastercatalog();

}






#endif // MASTERCATALOG_H
