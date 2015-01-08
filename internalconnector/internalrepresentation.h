#ifndef INTERNALREPRESENTATION_H
#define INTERNALREPRESENTATION_H

namespace Ilwis {
namespace Internal {


class InternalRepresentationConnector : public IlwisObjectConnector
{
public:
    InternalRepresentationConnector(const Resource &resource, bool load,const IOOptions& options=IOOptions());

    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    QString provider() const;
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
};
}
}

#endif // INTERNALREPRESENTATION_H
