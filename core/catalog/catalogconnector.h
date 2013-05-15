#ifndef CATALOGCONNECTOR_H
#define CATALOGCONNECTOR_H

#include "Kernel_global.h"

namespace Ilwis {

class Catalog;


class KERNELSHARED_EXPORT CatalogConnector : public ConnectorInterface
{
public:
    CatalogConnector(const Resource& item) : _location(item) {}
    virtual ~CatalogConnector() {}

    virtual bool loadItems() = 0;
    /*!
     \brief adds a filter to set of filters which this connector will use when adding items to a catalog

     The connector becomes owner of the filter has been added and ensure that it is finally deleted. No double filters allowed

     \param filter filter definition
    */
    QUrl location() const;
    ConnectorInterface* clone() const;

    virtual bool canUse(const QUrl& res) const = 0;

protected:
     Resource _location;
};

}

#endif // CATALOGCONNECTOR_H
