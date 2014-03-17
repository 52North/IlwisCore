#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include <QFileInfo>
#include "Kernel_global.h"
#include "kernel.h"
#include "ilwisdata.h"





namespace Ilwis {

class  CatalogConnector;
class CatalogFactory;


/*!
 The catalog class represents a view on the master-catalog. A catalog query on the master-catalog results in a catalog. The catalog it self doesnt contain any resources. The moment you request which
 resources are in the the catalog, it queries the master-catalog. This means that your view is always upto-date with what the master-catalog knows
 */
class KERNELSHARED_EXPORT Catalog : public IlwisObject
{
    Q_OBJECT

public:
    friend class CatalogConnector;

    /*!
     empty constructor
     */
    explicit Catalog();
    /*!
     copy constructor
     * \param cat
     */
    Catalog(const Resource& resource);
    ~Catalog();
    /*!
     returns the filtered items out of the master-catalog.
     * \return list of resources
     */
    std::vector<Resource> items() const;
    void addItems(const std::vector<Resource>& itemlist);
    virtual bool prepare();
    QString type() const;
    bool isValid() const ;
    IlwisTypes ilwisType() const;
    /*!
    translates a given name to a full resource locator valid within the context of the Catalog

     \return The full qualified resource
    */
    virtual QString resolve(const QString& name, IlwisTypes tp=itUNKNOWN) const;
    /*!
    Convenience method to retrieve the location the catalog is currently attached to.

     location are maintained by the connector, not by the catalog. But sometimes it is needed (ui)
     to need to know the location the catalog is attached to.

     \return The attached catalog resource
    */
    QUrl parentCatalog() const;
    void setParentCatalog(const QUrl& url);
    QUrl filesystemLocation() const;
    IlwisObject *clone();


protected:

private:
    void addItemsPrivate(const std::vector<Resource>& itemlist, bool doclear=false); //  same as addItems but this ignores readonly flag; only used internally
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
