#ifndef INTERNALPROJECTIONCONNECTOR_H
#define INTERNALPROJECTIONCONNECTOR_H


namespace Ilwis {
namespace Internal {


class InternalProjectionConnector : public IlwisObjectConnector
{
public:

    InternalProjectionConnector(const Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    QString provider() const;
};
}
}

#endif // INTERNALPROJECTIONCONNECTOR_H
