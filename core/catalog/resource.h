#ifndef RESOURCE_H
#define RESOURCE_H

#include "kernel_global.h"
#include <QVariant>
#include <QUrlQuery>

class QSqlRecord;
class QSqlQuery;

#define INTERNAL_CATALOG QString("ilwis://internalcatalog")
#define INTERNAL_CATALOG_URL (QUrl(INTERNAL_CATALOG))


namespace Ilwis {

class InternalDatabaseConnection;

/**
 * Design In Ilwis a Resource is a unique definition of data or processing that can be translated <br>
 * into a single IlwisObject or Catalog. For example the location to a shape file or a query <br>
 * on a certain database. A Resource is identified by a unique combination of location <br>
 * (an url) and type. Often a single data source can be mapped to several resources. E.g <br>
 * a GeoTiff can be mapped to a GridCoverage obviously but it can also be mapped to a <br>
 * CoordinateSystem as the data for a full definition of the CoordinateSystem is also part <br>
 * of the GeoTiff file. Resources can be sub divided into three categories: data objects, <br>
 * operation objects and catalogs. The data objects can be seen as “Ilwis objects light” <br>
 * and the catalogs are the containers for these data objects. Containers are defined as <br>
 * queries on the MasterCatalog). <br>
 * <br>
 * Resources expose the following properties <br>
 * - URL describing the location of the data source. URL’s may contain fragments<br>
 * to pinpoint internal resources inside a compound source (the ‘#’ notation of<br>
 * urls). <br>
 * - The type of the data defined by this resource.<br>
 * - The container location of the resource. For files this is often simply a location <br>
 * in the hierarchical file system but for services or remote data this may well be <br>
 * the base url from which the service is called. <br>
 * - Its spatial location/ bounding box. Not relevant for all data types but invaluable <br>
 * for others.<br>
 * - Creation and modification times. <br>
 * - Supporting objects (by reference). An object can only be understood in its <br>
 * relation to other objects. A raster without its georeference or domain offers <br>
 * significant less information <br>
 * - A resource has a unique runtime id that is shared by the Ilwis object or catalog <br>
 * object it represents. <br>
 * - A name. A string that can be used as reference to the object. Expressions <br>
 * use the name to select a resource. Names need not be system unique, <br>
 * though they need to be unique in the context they are used (e.g. A name in a <br>
 * expression must be unique in the context were the expression is valid, it isn’t <br>
 * necessarily unique in the context of the MasterCatalog). <br>
 * <br>
 * An instantiated IlwisObject is always coupled to a Resource. They share the same id
 */
class KERNELSHARED_EXPORT Resource : public Identity
{
public:
    /**
     * Constructs an empty Resource
     */
    Resource();

    /**
     * Copy constructor, other resource must not be null
     * @param resource the Resource that should be copied
     */
    Resource(const Resource& resource);

    /**
     * Creates a resource from an url and a type
     * Will fail if the url or the type is invalid/undefined
     *
     * @param url the url that should be used to create this resource
     * @param tp the type of the resource
     * @param isNew set to false if you are not making a new Resource defaults to true (singletons)
     */
    Resource(const QUrl& url, const QUrl &rawurl, quint64 tp, bool isNew=true);
    Resource(const QUrl &url, quint64 tp, bool isNew=true);

    /**
     * Creates a Resource from a description in the form of a namespace name
     *
     * @param name name of the resource can be an url or a code
     * @param tp the type of the resource
     * @param isNew set to false if you are not making a new Resource defaults to true (singletons)
     */
    Resource(const QString &name, quint64 tp, bool isNew=true);

    /**
     * Creates an resource from a file, the file has to be specified with a valid url
     *
     * @param tp the type of the resource
     * @param url The url to the file you want to load in this resource
     */
    //TODO docu correct?
    Resource(quint64 tp, const QUrl& url=INTERNAL_CATALOG_URL, const QUrl& rawUrl=QUrl());

    /**
     * Creates a new Resource from a Database record, this record should come from the mastercatalog
     * @param rec
     */
    Resource(const QSqlRecord& rec);

    virtual ~Resource();

    /**
     * Changes the name of this resource, can adapt the file name on the disk
     *
     * @param nm the new name of this Resource
     * @param adaptUrl set to false if you do not want to change the file name
     */
    void name(const QString& nm, bool adaptUrl=true, bool updateDatabase=true);
    QString name() const { return Identity::name(); }
    QString rawName() const;

    /**
     * override of the [] operator, allows you to query this resource for some of its data.<br>
     *
     * You can query either for an "id" or an "name", these will always work, any other property might fail.
     *
     * @param property the property that should be retrieved from the attributetable
     * @return the Value of the property or iUNDEF
     */
    QVariant operator[] (const QString& property) const;
    Resource property2Resource(const QString &propertyName, IlwisTypes type) const;

    /**
     * Checks whether this Resource has a certain property, a resource always has a "name" and a "id"
     *
     * @param property the property that should be checked
     * @return true if the property excists within this resource
     */
    bool hasProperty(const QString& property) const;

    /**
     * Adds a property to this resource, a propert is a combination of a key and a value. <br>
     * the key can also be seen as the name of the property.
     * @param key
     * @param value
     */
    void addProperty(const QString& key, const QVariant& value );

    /**
     * Removes a property and its value from this resource.
     * @param key the property key.
     */
    void removeProperty(const QString& key);

    /**
     * Query for the url of this Resource, the url points to the file used to created this Resource
     * @return An url to the file of this Resource
     */
    QUrl url(bool asRaw=false) const;

    /**
     * Changes The url of this Resource, and thus changes this whole Resource, as it will read the new file and refill all the fields using that.<br>
     *
     * The url has to be valid though.
     * @param url The new url, must be valid
     */
    void setUrl(const QUrl& url, bool asRaw=false, bool updateDatabase=true);

    /**
     * Gives access to the query when this Resource has been created with a url containing a query string. If there the url did
     * not contain a query string the returned QUrlQuery is empty. This however can be checked via hasUrlQuery() beforehand.
     *
     * @return the query part of the resource url.
     */
    QUrlQuery urlQuery() const;

    /**
     * Indicates if the Resource has been created with a url containing a query string. If so it can be accessed by urlQuery().
     *
     * @return true if the Resource url has query parameters, false otherwise.
     */
    bool hasUrlQuery() const;

    /**
     * Generates the path to the local file of this Resource (C://etc/etc/etc)
     *
     * @param relative set to true if you want to generate the path relative to the current working directory
     * @return the path to the local file, returns sUNDEF if something goes wrong
     */
    QString toLocalFile(bool relative=false) const;

    /**
     * Query for the url to the container of this Resource (the container of the file this Resource is based on, that is). <br>
     * in the case of a folder structure, level references the amount of folders you want to "go up", so if you folder structure is:<br>
     * 1\2\3\4\5\file.file<br>
     * and you enter as level 3, you get as url 1/2/
     *
     * @param level the amount of levels above the file
     * @return the url of the container specified
     */
    QUrl container(bool asRaw=false) const;

    /**
     * adds a container to this resource, defines the container this resource is in inside ilwis
     *
     * @param url The container that has to be set
     * @param level The level of the new container
     */
    void addContainer(const QUrl &url, bool asRaw=false);

    /**
     * Query for the size of this Resource
     *
     * @return the size of this Resource, cannot be <0
     */
    quint64 size() const;
    void size(quint64 objectsz);

    /**
     * Query this Resource for its dimensions. <br>
     * the dimension of a Resource is in general some form of size, could be in bytes, but stuff like XxY pixels is also possible
     * @return
     */
    QString dimensions() const;
    void dimensions(const QString& dim);

    /**
     * Returns the ilwis type of this Resource, anything is possible
     *
     * \sa IlwisObject
     * @return The correct IlwisType
     */
    IlwisTypes ilwisType() const;

    /**
     * Returns the extended IlwisType
     *
     * \sa IlwisObject
     * @return The correct IlwisType
     */
    IlwisTypes extendedType() const;

    /**
     * Sets the ilwistype of this Resource. <br>
     *
     * usefull in the case the type of this resource can not be resolved from the filename / extension
     *
     * \sa IlwisObject
     * @param tp the new ilwistype of this resource
     */
    void setIlwisType(IlwisTypes tp);

    /**
     * Sets the extendedType of this Resource
     *
     * \sa IlwisObject
     * @param tp the new extended type of this resource
     */
    void setExtendedType(IlwisTypes tp);

    /**
     * Prepares this Resource
     */
    virtual void prepare(quint64 base=1000000);

    /**
     * Store this resource in the sql database
     *
     * @param queryItem The item that should be stored
     * @param queryProperties The properties of the item that should be stored
     * @return true if stored succelful
     */
    bool store(Ilwis::InternalDatabaseConnection &queryItem, Ilwis::InternalDatabaseConnection &queryProperties) const;
    bool store(QDataStream& stream) const;
    bool load(QDataStream &stream);

    /**
     * Checks if this Resource is valid
     * a resource is valid if it has a valid name, a valid IlwisType and the url to the resource is valid
     *
     * @return true if this resource is valid
     */
    bool isValid() const;

    /**
     * override of the operater, checks whether this Resource is smaller than the supplied resource, returns true if this is the case. <br>
     * This method is for sorting purposes, it gets sorted first on url, and afterwards on ilwistype
     *
     * @param resource the resource this should be compared with
     * @return true if this<resource
     */
    bool operator()(const Ilwis::Resource& resource);

    /**
     * Sets a new id on this Resource. A new id has to be unique, and not registered in the catalog
     *
     * @param newid the new id of this resource
     */
    void setId(quint64 newid);

    /**
     * Creates a localfile path representing this Resource. <br>
     * The url must be valid.
     *
     * @param url The location of the file
     * @param relative set to true if you want a path relative to the workingcatalog
     * @return A Qurl to the file if succesful, or else a sUNDEF
     */
    static QString toLocalFile(const QUrl& url, bool relative=false, const QString &ext=sUNDEF);

    /**
     * Copies this resource, only difference is the id, since Resources are always singletons. <br>
     *
     * The new Resource will be prepared and than returned
     *
     * @param id the id of the copy
     * @return the copy
     */
    Resource copy(quint64 id) const;
    bool isRoot() const;
    double modifiedTime() const;
    void modifiedTime(const double& time, bool force=false);
    double createTime() const;
    void createTime(const double& time);
    static bool isRoot(const QString &txt) ;
    static QString quoted2string(const QString &name);


    bool hasChanged() const;
    void changed(bool yesno);
    void code(const QString &code);
    void setDescription(const QString &desc);
    QString code() const;
protected:
    void stringAsUrl(const QString& txt, IlwisTypes tp, bool isNew);
    QHash<QString, QVariant> _properties;
    QUrl _normalizedUrl;
    QUrl _rawUrl;
    QUrlQuery _urlQuery;
    QUrl _container;
    QUrl _rawContainer;
    quint64 _size;
    QString _dimensions;
    IlwisTypes _ilwtype;
    IlwisTypes _extendedType;
    double _modifiedTime = rUNDEF;
    double _createTime = rUNDEF;
    bool _changed = false;

    void checkUrl(IlwisTypes tp);

private:
    void setInternalCatalogPaths(IlwisTypes tp, const QString &txt);
};



KERNELSHARED_EXPORT uint qHash(const Ilwis::Resource& resource );
KERNELSHARED_EXPORT uint qHash2(const QUrl& url, IlwisTypes tp );
KERNELSHARED_EXPORT bool operator==(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );
KERNELSHARED_EXPORT bool operator!=(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );

}

Q_DECLARE_METATYPE(Ilwis::Resource)

#endif // RESOURCE_H
