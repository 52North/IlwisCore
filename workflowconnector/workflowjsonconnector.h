#ifndef WORKFLOWJSONCONNECTOR_H
#define WORKFLOWJSONCONNECTOR_H

#include <QBuffer>

namespace Ilwis {

class Workflow;

namespace WorkflowConnector {

    class WorkflowJSONConnector : public IlwisObjectConnector
    {
    public:
        static const int EXTERNAL_INPUT = 1;
        static const int ASSIGNED_INPUT = 2;
        static const int GENERATED_INPUT = 4;

        WorkflowJSONConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );
        virtual ~WorkflowJSONConnector() {}

        bool loadMetaData(IlwisObject*, const IOOptions & );
        bool loadData(IlwisObject*, const IOOptions& options = IOOptions());
        bool store(IlwisObject *, const IOOptions& options = IOOptions() );
        QString type() const;
        bool isReadOnly() const;
        Ilwis::IlwisObject *create() const;
        QString provider() const;
        QVariant getProperty(const QString& ) const;

        static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());

    private:
        QJsonObject createJSONWorkflow(const Resource & res);
        QJsonObject createJSONWorkflowMetaData(const Resource& res);
        QJsonObject createJSONOperationMetadata(const SPWorkFlowNode& node, const std::vector<SPWorkFlowNode>& outNodes);
        QJsonObject createJSONRangeMetadata(const SPWorkFlowNode& node);
        QJsonArray createJSONOperationInputList(const SPWorkFlowNode& node);
        QJsonArray createJSONOperationOutputList(const Ilwis::SPWorkFlowNode &node);
        QJsonArray createJSONOperationConnectionList(Ilwis::Workflow *workflow, std::map<quint64, int> nodeMapping);
        bool openTarget();

        std::unique_ptr<QIODevice> _datasource;

        JsonConfig _config;

        QByteArray _data;
        QMap<QString, QString> _layer2LocalLUT;
        QSet<int> _leafOperations;

//        void writeWMSLocalLUT(QString filename);
    };

}
}

#endif // WORKFLOWJSONCONNECTOR_H
