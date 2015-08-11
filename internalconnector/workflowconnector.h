#ifndef WORKFLOWCONNECTOR_H
#define WORKFLOWCONNECTOR_H

namespace Ilwis {
namespace Internal {


class WorkflowConnector : public IlwisObjectConnector
{
public:
    WorkflowConnector();

    WorkflowConnector(const Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *, const IOOptions& options = IOOptions());
    QString provider() const;
};
}
}

#endif // WORKFLOWCONNECTOR_H
