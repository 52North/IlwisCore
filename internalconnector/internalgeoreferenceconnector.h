#ifndef INTERNALGEOREFERENCECONNECTOR_H
#define INTERNALGEOREFERENCECONNECTOR_H

namespace Ilwis {
namespace Internal {
class InternalGeoReferenceConnector : public IlwisObjectConnector
{
public:
    InternalGeoReferenceConnector();

    InternalGeoReferenceConnector(const Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject* data,const PrepareOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadData(IlwisObject *, const LoadOptions& options = LoadOptions());
    QString provider() const;
};
}
}

#endif // INTERNALGEOREFERENCECONNECTOR_H
