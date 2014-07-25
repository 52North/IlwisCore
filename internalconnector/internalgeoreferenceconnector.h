#ifndef INTERNALGEOREFERENCECONNECTOR_H
#define INTERNALGEOREFERENCECONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalGeoReferenceConnector : public IlwisObjectConnector
{
public:
    InternalGeoReferenceConnector();

    InternalGeoReferenceConnector(const Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    QString provider() const;
};
}
}

#endif // INTERNALGEOREFERENCECONNECTOR_H
