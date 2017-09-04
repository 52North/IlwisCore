#ifndef DOMAINCONNECTOR_H
#define DOMAINCONNECTOR_H

namespace Ilwis{
namespace Gdal{
class DomainConnector : public GdalConnector
{
public:
    DomainConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);

    static ConnectorInterface *create(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    IlwisObject *create() const;
private:
    bool handleThematicDomains(IlwisObject *);
    bool handleValueDomains(IlwisObject *data);

};
}
}

#endif // DOMAINCONNECTOR_H
