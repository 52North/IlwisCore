#ifndef MASTERCATALOG_H
#define MASTERCATALOG_H

#include <QMultiMap>
#include <set>
#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class CatalogConnector;

typedef std::shared_ptr<IlwisObject> ESPIlwisObject;
/*!
 Master catalog holds references to all discovered data files, operations and to all instantiated ilwis objects
 The master catalog is basically a database and all information about resources that ilwis can handle can be
 found in this database. The fields in the database are a reflection of the fields in the resource class and the
 variable set of properties each resource can have.
 Main function of the mastercatalog is
 */
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
    quint64 name2id(const QString& name, IlwisTypes tp= itUNKNOWN) const;
    IlwisTypes id2type(quint64 id) const;
    quint64 createId() const;
    void registerObject(ESPIlwisObject &data);
    bool isRegistered(quint64 id) const;
    bool unregister(quint64);
    std::list<Resource> select(const QUrl& res, const QString& selection) const;

    QUrl name2url(const QString &name, IlwisTypes tp=itUNKNOWN) const;
    ESPIlwisObject get(const QUrl &resource, IlwisTypes type) const;
    ESPIlwisObject get(quint64 id) const;
    bool contains(const QUrl &url, IlwisTypes type) const;

private:
    static MasterCatalog *_masterCatalog;
    quint64 _baseid;
    QHash<quint64, ESPIlwisObject> _lookup;
    QMultiMap<QUrl, CatalogConnector*  > _catalogs;
    std::set<uint> _knownHashes;


};

//typedef QHash<IlwisResource, QList<CatalogCreate>  > CatalogCollection;

KERNELSHARED_EXPORT MasterCatalog* mastercatalog();

}






#endif // MASTERCATALOG_H
