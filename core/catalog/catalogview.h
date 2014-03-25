#ifndef CATALOGVIEW_H
#define CATALOGVIEW_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include <QFileInfo>
#include "Kernel_global.h"
#include "identity.h"
#include "resource.h"
//#include "connectorinterface.h"
//#include "catalogconnector.h"


namespace Ilwis {

class  CatalogConnector;
class CatalogFactory;


/*!
 The catalog class represents a view on the master-catalog. A catalog query on the master-catalog results in a catalog. The catalog it self doesnt contain any resources. The moment you request which
 resources are in the the catalog, it queries the master-catalog. This means that your view is always upto-date with what the master-catalog knows
 */
class KERNELSHARED_EXPORT CatalogView : public QObject, public Identity
{
    Q_OBJECT

public:

    /*!
     empty constructor
     */
    explicit CatalogView(QObject *parent = 0);
    /*!
     copy constructor
     * \param cat
     */
    CatalogView(const CatalogView& cat);
    ~CatalogView(){}
    /*!
     returns the filtered items out of the master-catalog.
     * \return list of resources
     */
    std::vector<Resource> items() const;
    virtual bool prepare(const QUrl &resource, const QString &filter="");
    QString type() const;
    bool isValid() const ;
     /*!
    Convenience method to retrieve the location the catalog is currently attached to.

     location are maintained by the connector, not by the catalog. But sometimes it is needed (ui)
     to need to know the location the catalog is attached to.

     \return The attached catalog resource
    */
    QUrl parentCatalogView() const;
    void setParentCatalogView(const QUrl& url);


protected:
    bool fillCatalog();

    QString _filter;
    QUrl _location;
    QUrl _parent;

    
signals:
    
public slots:
    
};
}

typedef QSharedPointer<Ilwis::CatalogView> SPCatalog;

Q_DECLARE_METATYPE(Ilwis::CatalogView)

#endif // CATALOG_H
