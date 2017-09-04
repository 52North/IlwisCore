#ifndef WorkflowOBJECTFACTORY_H
#define WorkflowOBJECTFACTORY_H

namespace Ilwis {
namespace WorkflowConnector {

class WorkflowConnectorObjectFactory : public IlwisObjectFactory
{
public:
    WorkflowConnectorObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
private:
};
}
}

#endif // WorkflowOBJECTFACTORY_H
