#ifndef CATALOGVIEW_H
#define CATALOGVIEW_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include <QFileInfo>
#include <set>
#include "kernel_global.h"
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
    CatalogView(const QUrl &loc);
    CatalogView(const Resource &resource);
    ~CatalogView(){}

    virtual bool prepare();
    CatalogView& operator=(const CatalogView& view);

    void addLocation(const QUrl &loc);
    QString type() const;
    bool isValid() const ;
    std::vector<Resource> items() const;
    void addFixedItem(quint64 id);
    void removeFixedItem(quint64 id);
    quint32 fixedItemCount() const;
    Resource resource() const;
    bool hasParent() const;
    bool isActiveFilter(const QString& name) const;
    void filter(const QString& filterName, const QString& filterString);
    QString filter(const QString &name) const;
    void addActiveFilter(const QString& filterName);
    bool removeActiveFilter(const QString& filterName);
    void filterChanged(const QString &typeIndication, bool state);
    int filterCount() const;
    IlwisTypes objectFilter() const;
    void storeFilters() const;

     /*!
    Convenience method to retrieve the location the catalog is currently attached to.

     location are maintained by the connector, not by the catalog. But sometimes it is needed (ui)
     to need to know the location the catalog is attached to.

     \return The attached catalog resource
    */
    QUrl parentCatalogView() const;
    void setParentCatalogView(const QUrl& url);
    void setFilterState(bool state);



protected:
    struct FilterItem{
        FilterItem() {}
        FilterItem(const QString& name,const QVariant& filter):_filter(filter),_filterName(name){}
        QVariant _filter;
        QString _filterName;
    };

    bool fillCatalog();

    std::map<QString,FilterItem> _filters;
    std::set<QString> _activeFilters;
    std::vector<QUrl> _locations;
    std::map<quint64, Resource> _fixedItems;
    std::map<IlwisTypes, bool> _filterState;
    QUrl _parent;
    Resource _resource;

    
signals:
    
public slots:
    
};
}

typedef QSharedPointer<Ilwis::CatalogView> SPCatalog;

Q_DECLARE_METATYPE(Ilwis::CatalogView)

#endif // CATALOG_H
