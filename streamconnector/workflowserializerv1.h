#ifndef WORKFLOWSERIALIZERV1_H
#define WORKFLOWSERIALIZERV1_H

namespace Ilwis {
class WorkFlowNode;
class Workflow;

typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;

namespace Stream {

class WorkflowSerializerV1 : public OperationMetadataSerializerV1
{
public:
    WorkflowSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static VersionedSerializer *create(QDataStream &stream);
private:
    bool storeNode(const Ilwis::SPWorkFlowNode &node, const IOOptions &options=IOOptions());
    void loadNode(Ilwis::SPWorkFlowNode &node, Workflow *workflow, const IOOptions &options=IOOptions());
    void storeNodeLinks(const Ilwis::SPWorkFlowNode &node);
    void loadNodeLinks(SPWorkFlowNode &node, Workflow *workflow);
};
}
}

#endif // WORKFLOWSERIALIZERV1_H
