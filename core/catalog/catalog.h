#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include "Kernel_global.h"
#include "identity.h"
#include "resource.h"
#include "connectorinterface.h"
#include "catalogconnector.h"


namespace Ilwis {

class  CatalogConnector;
class CatalogFactory;


/*!
 The catalog class represents a view on the master-catalog. A catalog query on the master-catalog results in a catalog. The catalog it self doesnt contain any resources. The moment you request which
 resources are in the the catalog, it queries the master-catalog. This means that your view is always upto-date with what the master-catalog knows
 */
class KERNELSHARED_EXPORT Catalog : public QObject, public Identity
{
    Q_OBJECT

public:

    /*!
     empty constructor
     */
    explicit Catalog(QObject *parent = 0);
    /*!
     copy constructor
     * \param cat
     */
    Catalog(const Catalog& cat);
    /*!
     returns the filtered items out of the master-catalog.
     * \return list of resources
     */
    std::list<Resource> items() const;
    virtual bool prepare(const QUrl &resource, const QString &filter="");
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
    QUrl location() const;
    QUrl filesystemLocation() const;


protected:
    bool fillCatalog();

    QString _filter;
    QUrl _location;
    QUrl _parent;

    
signals:
    
public slots:
    
};
}

typedef QSharedPointer<Ilwis::Catalog> SPCatalog;

Q_DECLARE_METATYPE(Ilwis::Catalog)

#endif // CATALOG_H
