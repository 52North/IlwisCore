#ifndef MASTERCATALOG_H
#define MASTERCATALOG_H

#include <QMultiMap>
#include <QObject>
#include <QSqlQuery>
#include <set>
#include <memory>
#include <mutex>
#include "ilwis.h"
#include "ilwistypes.h"
#include "kernel_global.h"

typedef std::set<QUrl> UrlSet;

namespace Ilwis {

class Resource;
class CatalogConnector;
class Catalog;
class IlwisObject;
class CoordinateSystem;

typedef std::shared_ptr<IlwisObject> ESPIlwisObject;
/*!
 Master catalog holds references to all discovered data files, operations and to all instantiated ilwis objects
 The master catalog is basically a database and all information about resources that ilwis can handle can be
 found in this database. The fields in the database are a reflection of the fields in the resource class and the
 variable set of properties each resource can have.
 */
class KERNELSHARED_EXPORT MasterCatalog : public QObject
{
    Q_OBJECT

    friend KERNELSHARED_EXPORT MasterCatalog* mastercatalog();

public:
    /**
     * Constructs a new MasterCatalog
     */
    MasterCatalog();

    ~MasterCatalog();

    /**
     * prepares the mastercatalog for use by loading all the settings
     *
     * @return true when succesful
     */
    bool prepare();

    /**
     * Adds a container with Resources to the MasterCatalog (think about a folder)
     *
     * @param location the url to the container
     * @return true if the adding of the container was succesfull
     */
    bool addContainer(const QUrl &location, bool forceScan=false);
    std::vector<Resource> addContainerContent(const QString &container, const std::vector<Resource> &items);

    /**
     * Adds a list of Resources to the MasterCatalog
     * @param items the Resources that should be added
     * @return true when succesful
     */
    bool addItems(const std::vector<Ilwis::Resource> &items, bool silent=false);
    bool updateItems(const std::vector<Resource> &iteme, bool silent=false);


    /**
     * Removes a list of Resources from the MasterCatalog
     * @param items The Resources that should be removed from the MasterCatalog
     * @return true if succesful
     */
    bool removeItems(const std::vector<Ilwis::Resource> &items);

    /**
     * Translates an url that is in the MasterCatalog to the correct id
     *
     * @param url the url that has to be translated, must be in the MasterCatalog
     * @param tp the ilwisType that belongs to the url
     * @return the id of the given url
     */
    quint64 url2id(const QUrl& url, IlwisTypes tp, bool casesensitive=true) const;

    /**
     * Translates a name that is in the MasterCatalog to the corret Resource
     *
     * this look up might fail, even though the conditions are met
     * @param name the name that has to be translated, must be in the MasterCatalog
     * @param tp the IlwisType that belongs to the name, defaults to itUNKOWN
     * @return The Resource of the given name
     */
    Resource name2Resource(const QString& name, IlwisTypes tp = itUNKNOWN) const;

    /**
     * Translates an id that is in the MasterCatalog to correct Resource
     *
     * @param iid the id that has to be translated,must be in the MasterCatalog
     * @return The Resource of the given id
     */
    Resource id2Resource(quint64 iid) const;

    /**
     * Translates a name that is in the MasterCatalog to the corret id
     *
     * this look up might fail, even though the conditions are met
     * @param name the name that has to be translated, must be in the MasterCatalog
     * @param tp the IlwisType that belongs to the name, defaults to itUNKOWN
     * @return The id of the given name
     */
    quint64 name2id(const QString& name, IlwisTypes tp= itUNKNOWN) const;

    bool changeResource(quint64 objectid, const QString& attribute, const QVariant& var, bool extended=false);
    /**
     * Translates an id from the MasterCatalog to the correct IlwisType
     * @param id the id that needs translation
     * @return the IlwisType of the given id
     */
    IlwisTypes id2type(quint64 id) const;

    /**
     * Generates a new unique id, which is not yet used by any resource
     * @return an new id
     */
    quint64 createId() const;

    /**
     * Registers an new object in the MasterCatalog
     * @param data the data that forms the new object
     */
    void registerObject(ESPIlwisObject &data);

    /**
     * Checks whether a certain id is registered with an Resource in the MasterCatalog
     * @param id the id that should be checked
     * @return true when the given id is in use
     */
    bool isRegistered(quint64 id) const;

    /**
     * unregisters the given id and removes the related object from the MasterCatalog
     *
     * @return true when succesful
     */
    bool unregister(quint64);

    /**
     * Selects multiple records from the MasterCatalog
     * selection language not yet finished
     * @param resource
     * @param selection
     * @return a list of the selected resources
     */
    //TODO select language, finish docu
    std::vector<Resource> select(const QUrl& resource, const QString& selection) const;
    std::vector<Resource> select(const QString &selection) const;

    /**
     * Translates a name from the MasterCatalog to the correct url
     *
     * this look up might fail, even though the conditions are met
     * @param name the name that has to be translated, must be in the MasterCatalog
     * @param tp tp the IlwisType that belongs to the name, defaults to itUNKOWN
     * @return the url of the given name
     */
    QUrl name2url(const QString &name, IlwisTypes tp=itUNKNOWN) const;

    /**
     * Retrieves an ilwisobject from the MasterCatalog based on the url en the ilwisttype
     * @param resource the url to the resource that should be gotten
     * @param type the type of the resource
     * @return an ESPIlwisObject based on the url
     */
    ESPIlwisObject get(const QUrl &resource, IlwisTypes type) const;

    /**
     * Retrieve an item from the mastercatalog with its id
     * @param id the id of the required item
     * @return an ESPIlwisObject with the correct id
     */
    ESPIlwisObject get(quint64 id) const;

    /**
     * Checks whether a certain url + IlwisType combination is in the MasterCatalog
     *
     * @param url the url
     * @param type the ilwistype
     * @return true when the combination is in the MasterCatalog
     */
    bool contains(const QUrl &url, IlwisTypes type) const;
    bool contains(quint64 id) const;
    bool knownCatalogContent(const QUrl& path) const;

    bool usesContainers(const QUrl &scheme) const;
    void addContainerException(const QString& scheme);


#ifdef QT_DEBUG
    quint32 lookupSize() const { return _lookup.size(); }
    const QHash<quint64, ESPIlwisObject>& dumpLookup() const;
    quint32 usecount(quint64 id);
    void ilwisDataDebug(const ESPIlwisObject& obj) const;

#endif
    static const QString MASTERCATALOG;

signals:
    void contentChanged(const UrlSet& locs);
private:
    static MasterCatalog *_masterCatalog;
    QHash<quint64, ESPIlwisObject> _lookup;
    std::set<QUrl> _catalogs;
    std::set<uint> _knownHashes;
    std::set<QString> _containerExceptions; // for some schemes the mastercatelog shouldnt try to find containers as they dont make sense;
    mutable std::recursive_mutex _guard;
    QString replaceSymbols(const QString &selection) const;
};

//typedef QHash<IlwisResource, QList<CatalogCreate>  > CatalogCollection;

KERNELSHARED_EXPORT MasterCatalog* mastercatalog();

class KERNELSHARED_EXPORT CalcLatLon {
public:
    static void calculatelatLonEnvelopes(const QString& query, const QString& name);
    static void calculatelatLonEnvelopes(std::vector<Resource> &resources, const QString &name);
};

}






#endif // MASTERCATALOG_H
