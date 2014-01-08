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
     * @brief Resource
     * @param rec
     */
    Resource(const QSqlRecord& rec);

    virtual ~Resource() {}

    /**
     * Changes the name of this resource, can
     * @param nm
     * @param adaptUrl
     */
    void setName(const QString& nm, bool adaptUrl=true);

    /**
     * @brief operator []
     * @param property
     * @return
     */
    QVariant operator[] (const QString& property) const;

    /**
     * @brief hasProperty
     * @param property
     * @return
     */
    bool hasProperty(const QString& property) const;

    /**
     * @brief addProperty
     * @param key
     * @param value
     */
    void addProperty(const QString& key, const QVariant& value );

    /**
     * @brief url
     * @return
     */
    QUrl url() const;

    /**
     * @brief setUrl
     * @param url
     */
    void setUrl(const QUrl& url);

    /**
     * @brief toLocalFile
     * @param relative
     * @return
     */
    QString toLocalFile(bool relative=false) const;

    /**
     * @brief container
     * @param level
     * @return
     */
    QUrl container(int level=0) const;

    /**
     * @brief addContainer
     * @param url
     * @param level
     */
    void addContainer(const QUrl &url, int level=0);

    /**
     * @brief size
     * @return
     */
    quint64 size() const;

    /**
     * @brief dimensions
     * @return
     */
    QString dimensions() const;

    /**
     * @brief ilwisType
     * @return
     */
    IlwisTypes ilwisType() const;

    /**
     * @brief extendedType
     * @return
     */
    IlwisTypes extendedType() const;

    /**
     * @brief setIlwisType
     * @param tp
     */
    void setIlwisType(IlwisTypes tp);

    /**
     * @brief setExtendedType
     * @param tp
     */
    void setExtendedType(IlwisTypes tp);

    /**
     * @brief prepare
     */
    void prepare();

    /**
     * @brief store
     * @param queryItem
     * @param queryProperties
     * @return
     */
    bool store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief operator ()
     * @param resource
     * @return
     */
    bool operator()(const Ilwis::Resource& resource);

    /**
     * @brief setId
     * @param newid
     */
    void setId(quint64 newid);

    /**
     * @brief toLocalFile
     * @param url
     * @param relative
     * @return
     */
    static QString toLocalFile(const QUrl& url, bool relative=false);

    /**
     * @brief copy
     * @param id
     * @return
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
