#ifndef INTERNALELLIPSOIDCONNECTOR_H
#define INTERNALELLIPSOIDCONNECTOR_H


namespace Ilwis {
namespace Internal {


class InternalEllipsoidConnector : public IlwisObjectConnector
{
public:

    InternalEllipsoidConnector(const Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    QString provider() const;
};
}
}

#endif // INTERNALELLIPSOIDCONNECTOR_H
