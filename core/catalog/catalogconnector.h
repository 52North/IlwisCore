#ifndef CATALOGCONNECTOR_H
#define CATALOGCONNECTOR_H

#include "Kernel_global.h"
#include <QStringList>

namespace Ilwis {

class Catalog;
class ContainerConnector;


class KERNELSHARED_EXPORT CatalogConnector : public ConnectorInterface
{
public:
    CatalogConnector(const Resource& resource);
    virtual ~CatalogConnector() {}

    virtual bool loadItems() = 0;
    virtual QStringList itemlist() const {return QStringList();}
    /*!
     \brief adds a filter to set of filters which this connector will use when adding items to a catalog

     The connector becomes owner of the filter has been added and ensure that it is finally deleted. No double filters allowed

     \param filter filter definition
    */
    ConnectorInterface* clone() const;

    virtual bool canUse(const Resource& resource) const = 0;
    Resource& source();
    bool isValid() const;
protected:
     Resource _location;

     std::unique_ptr<ContainerConnector>& containerConnector();
     const std::unique_ptr<ContainerConnector>& containerConnector() const;

private:
    std::unique_ptr<ContainerConnector> _containerconnector;
};

}

#endif // CATALOGCONNECTOR_H
