#ifndef RESOURCE_H
#define RESOURCE_H

#include "Kernel_global.h"
#include <QVariant>

class QSqlRecord;
class QSqlQuery;

#define INTERNAL_OBJECT (QUrl("ilwis://internalcatalog"))


namespace Ilwis {
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
    Resource(const QUrl& url, quint64 tp, bool isNew=true);

    /**
     * Creates a Resource
     * @param code
     * @param tp
     * @param isNew
     *-=-//inconsistent parameters .c en.h, idk which name to trust, code or name?
     */
    Resource(const QString &code, quint64 tp, bool isNew=true);

    /**
     * @brief Resource
     * @param tp
     * @param url
     *-=-
     */
    Resource(quint64 tp, const QUrl& url=INTERNAL_OBJECT);

    /**
     * Creates a new Resource from a Database record, this record should come from the mastercatalog
     * @param rec
     */
    Resource(const QSqlRecord& rec);

    virtual ~Resource() {}

    /**
     * Changes the name of this resource, can adapt the file name on the disk
     *
     * @param nm the new name of this Resource
     * @param adaptUrl set to false if you do not want to change the file name
     */
    void setName(const QString& nm, bool adaptUrl=true);

    /**
     * override of the [] operator, allows you to query this resource for some of its data.<br>
     *
     * You can query either for an "id" or an "name", these will always work, any other property might fail.
     *
     * @param property the property that should be retrieved from the attributetable
     * @return the Value of the property or iUNDEF
     */
    QVariant operator[] (const QString& property) const;

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
     * Query for the url of this Resource, the url points to the file used to created this Resource
     * @return An url to the file of this Resource
     */
    QUrl url() const;

    /**
     * Changes The url of this Resource, and thus changes this whole Resource, as it will read the new file and refill all the fields using that.<br>
     *
     * The url has to be valid though.
     * @param url The new url, must be valid
     */
    void setUrl(const QUrl& url);

    /**
      -=-?
     * @brief toLocalFile
     * @param relative
     * @return
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
    QUrl container(int level=0) const;

    /**
      -=- dont know details
     * @brief addContainer
     * @param url
     * @param level
     */
    void addContainer(const QUrl &url, int level=0);

    /**
     * Query for the size of this Resource
     *
     * @return the size of this Resource, cannot be <0
     */
    quint64 size() const;

    /**
     * Query this Resource for tis dimensions
     * -=- ?
     * @return
     */
    QString dimensions() const;

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
      -=- conditions?
     * Sets the ilwistype of this Resource
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
    void prepare();

    /**
     * -=- ?
     * @param queryItem
     * @param queryProperties
     * @return
     */
    bool store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const;

    /**
     * Checks if this Resource is valid
     *
     * a resource is valid if it has a valid name, a valid IlwisType and the url to the resource is valid
     *
     *
     * @return true if this resource is valid
     */
    bool isValid() const;

    /**
      -=- what does < do?, copy constructor?
     * @brief operator ()
     * @param resource
     * @return
     */
    bool operator()(const Ilwis::Resource& resource);

    /**
     * Sets a new id on this Resource. A new id has to be unique, and not registered in the catalog
     *
     * @param newid the new id of this resource
     */
    void setId(quint64 newid);

    /**
     * Creates a localfile representing this Resource. <br>
     * The url must be valid, and is should also represent the directory the file should be placed
     *
     * @param url The location of the file
     * @param relative -=- what does it mean?>
     * @return A Qurl to the file if succesful, or else a sUNDEF
     */
    static QString toLocalFile(const QUrl& url, bool relative=false);

    /**
     * Copies this resource, only difference is the id, since Resources are always singletons. <br>
     *
     * The new Resource will be prepared and than returned
     *
     * @param id the id of the copy
     * @return the copy
     */
    Resource copy(quint64 id) const;

protected:
    void stringAsUrl(const QString& txt, IlwisTypes tp, bool isNew);
    QHash<QString, QVariant> _properties;
    QUrl _resource;
    std::vector<QUrl> _container;
    quint64 _size;
    QString _dimensions;
    IlwisTypes _ilwtype;
    IlwisTypes _extendedType;

    void checkUrl(IlwisTypes tp);

};



KERNELSHARED_EXPORT uint qHash(const Ilwis::Resource& resource );
KERNELSHARED_EXPORT uint qHash2(const QUrl& url, IlwisTypes tp );
KERNELSHARED_EXPORT bool operator==(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );

}

#endif // RESOURCE_H
