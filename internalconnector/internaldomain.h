#ifndef INTERNALDOMAIN_H
#define INTERNALDOMAIN_H

namespace Ilwis {
namespace Internal {


class InternalDomainConnector : public IlwisObjectConnector
{
public:

    InternalDomainConnector(const Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject* data,const PrepareOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadData(IlwisObject *);
    QString provider() const;
};
}
}

#endif // INTERNALDOMAIN_H
