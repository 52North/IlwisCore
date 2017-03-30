#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include <QFileInfo>
#include "kernel_global.h"
#include "kernel.h"
#include "ilwisdata.h"





namespace Ilwis {

class  CatalogConnector;
class CatalogFactory;

/*!
 The catalog class holds the collection of data in a certain data-source. Catalogs come in many shapes and sizes. Basically anything that can contain other
 Ilwis-objects is a catalog. The obvious example if of course a folder on the harddrive, but also file-formats that contain other objects can be considered catalogs, or services
 that give access to coverage, or ftp  network locations etc... Note that the class \ref Ilwis.CatalogView implements a filtered view on catalog(s).
 */
class KERNELSHARED_EXPORT Catalog : public IlwisObject
{
    Q_OBJECT

public:
    friend class CatalogConnector;

    /*!
     empty constructor. The object is in an invalid state
     */
    explicit Catalog();
    /*!
     Attaches a Catalog to a location described in the resource. The object is not yet filled. This happens in the prepare method.
     * \param cat
     */
    Catalog(const Resource& resource);
    ~Catalog();
    /*!
     * \return list of resources
     */
    /**
     * @brief Collection of ilwis-objects contained in this catalog
     *
     * Returns the unfiltered collection of  items that have this catalog ias container. Note that an item may itself be a catalog so a hierachical  tree can be constructed this way.
     * @return list of resources contained in the catalog
     */
    std::vector<Resource> items() const;
    quint32 itemCount() const;
    void scan();

    template<typename IObjectType=IIlwisObject> IObjectType item(const QVariant& indexValue) const{
        IIlwisObject obj = createObject(indexValue)   ;
        if ( obj.isValid() )
            return obj.as<IObjectType>();
        return IObjectType();
    }
    /**
     * @brief Brings the Catalog in a valid state
     *
     * The prepare method executes the loadData method of the \ref Ilwis.CatalogConnector that backs the Catalog object. A succesfull loadData will make the Catalog valid.
     *
     * @return bool
     */
    virtual bool prepare(const IOOptions& options=IOOptions());
    /**
     * @brief Indicates if a Catalog is in a valid state
     *
     * A Catalog is in a valid state if the url that describes its location is valid and the objects items are loaded. Having no items doesnt mean that the object is not in
     * a valid state because the catalog might be empty.
     *
     * @return bool
     */
    bool isValid() const ;
    /**
     * @brief return the type (as 64-integer). types are described in \ref Ilwis::IlwisObject
     *
     * @return IlwisTypes
     */
    IlwisTypes ilwisType() const;
    /*!
    translates a given name to a full resource locator valid within the context of the Catalog

     \return The full qualified resource
    */
    virtual QString resolve(const QString& nm, IlwisTypes tp=itUNKNOWN) const;
    /*!
    Retrieves the location the catalog is currently attached to.

     location are maintained by the connector, not by the catalog. But sometimes it is needed
     to need to know the location the catalog is attached to. Catalogs may form a hierarchical system.

     \return The attached catalogs url
    */
    QUrl parentCatalog() const;
    /**
     * @brief sets the Catalog this Catalog is part of.
     *
     * @param url url of the containing parent Catalog
     */
    void setParentCatalog(const QUrl& url);
    /**
     * @brief convenience method for translating the location of the catalog into a valid file based location
     *
     * Some of the underlying librairies of the framework can only handle files. This means that data that is not contained in a regular file/folder based
     *structure but still is basically a file can not be handled by the framework (e.g. files in a zip file). This method translates the url inot a location in the file system that
     * contains a (temporary) copy of the file(s) in that container. In this way the underlying library still can handle the data as-if it was a regular file.
     *
     * @return QUrl
     */
    QUrl filesystemLocation() const;

    IlwisObject *clone();
    void unload();
    std::vector<QString> explorerProviders();

    static const QString DEFAULT_WORKSPACE;
    static bool isSpatialDataLocation(const QUrl& url);

protected:
    virtual IIlwisObject createObject(const QVariant& indexValue) const;
    Resource item(quint32 index) const;

private:
    void addItemsPrivate(const std::vector<Resource>& itemlist, bool doclear=false); //  ignores readonly flag; only used internally
    bool fillCatalog();
    void copyTo(IlwisObject *obj);

    QUrl _parent;
    std::vector<Resource> _items;

    
signals:
    
public slots:
    
};
typedef Ilwis::IlwisData<Ilwis::Catalog> ICatalog;
}



Q_DECLARE_METATYPE(Ilwis::ICatalog)

#endif // CATALOG_H
